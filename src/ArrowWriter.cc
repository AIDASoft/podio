#include "podio/ArrowWriter.h"

#include "podio/CollectionBase.h"
#include "podio/DatamodelRegistry.h"
#include "podio/Frame.h"
#include "podio/podioVersion.h"
#include "podio/utilities/ArrowFrameConverter.h"

#include <arrow/io/file.h>
#include <arrow/table.h>
#include <arrow/type.h>
#include <nlohmann/json.hpp>
#include <parquet/arrow/writer.h>
#include <parquet/properties.h>

#include <algorithm>
#include <fstream>
#include <stdexcept>

namespace podio {

ArrowWriter::ArrowWriter(const std::string& directory, const Options& options) :
    m_directory(directory), m_options(options) {
  if (std::filesystem::exists(m_directory) && !std::filesystem::is_empty(m_directory)) {
    throw std::runtime_error("Output directory " + directory + " exists and is not empty.");
  }
  std::filesystem::create_directories(m_directory);
}

ArrowWriter::ArrowWriter(const std::string& directory) : ArrowWriter(directory, Options{}) {
}

ArrowWriter::~ArrowWriter() {
  try {
    finish();
  } catch (...) {
  }
}

void ArrowWriter::writeFrame(const podio::Frame& frame, std::string_view category) {
  writeFrame(frame, category, frame.getAvailableCollections());
}

void ArrowWriter::writeFrame(const podio::Frame& frame, std::string_view category,
                             const std::vector<std::string>& collsToWrite) {
  std::string catName(category);
  auto it = m_categories.find(catName);

  std::vector<std::string> sortedColls = collsToWrite;
  std::sort(sortedColls.begin(), sortedColls.end());

  if (it == m_categories.end()) {
    CategoryInfo catInfo;
    catInfo.filePath = m_directory / (catName + ".parquet");
    catInfo.collsToWrite = sortedColls;

    for (const auto& name : sortedColls) {
      const auto* coll = frame.get(name);
      if (!coll) {
        throw std::runtime_error("Collection " + name + " not found in frame.");
      }
      catInfo.collTypes.push_back(std::string(coll->getValueTypeName()));
      catInfo.collIsSubset.push_back(coll->isSubsetCollection());
      catInfo.collSchemaVersions.push_back(coll->getSchemaVersion());
      catInfo.collIDs.push_back(coll->getID());
      m_datamodelCollector.registerDatamodelDefinition(coll, name);
    }

    m_categories.emplace(catName, std::move(catInfo));
    it = m_categories.find(catName);
  } else {
    validateSchema(it->second, frame, sortedColls);
  }

  auto table = podio::convertFrameToTable(frame, sortedColls);
  if (!it->second.schema) {
    it->second.schema = table->schema();
  } else if (!it->second.schema->Equals(*table->schema())) {
    throw std::runtime_error("Schema drift detected: Arrow schema differs for subsequent frames.");
  }

  it->second.buffer.push_back(table);
  it->second.entries++;

  if (it->second.buffer.size() >= m_options.maxBufferedRows) {
    flushCategory(it->second);
  }
}

void ArrowWriter::validateSchema(const CategoryInfo& catInfo, const podio::Frame& frame,
                                 const std::vector<std::string>& collsToWrite) {
  if (catInfo.collsToWrite != collsToWrite) {
    throw std::runtime_error("Schema drift detected: collection names differ for subsequent frames.");
  }

  for (size_t i = 0; i < collsToWrite.size(); ++i) {
    const auto* coll = frame.get(collsToWrite[i]);
    if (!coll) {
      throw std::runtime_error("Collection " + collsToWrite[i] + " not found in frame.");
    }
    if (catInfo.collTypes[i] != coll->getValueTypeName() || catInfo.collIsSubset[i] != coll->isSubsetCollection() ||
        catInfo.collSchemaVersions[i] != coll->getSchemaVersion() || catInfo.collIDs[i] != coll->getID()) {
      throw std::runtime_error("Schema drift detected for collection: " + collsToWrite[i]);
    }
  }
}

void ArrowWriter::flushCategory(CategoryInfo& catInfo) {
  if (catInfo.buffer.empty()) {
    return;
  }

  auto result = arrow::ConcatenateTables(catInfo.buffer);
  if (!result.ok()) {
    throw std::runtime_error("Failed to concatenate arrow tables: " + result.status().ToString());
  }
  auto table = result.ValueOrDie();

  if (!catInfo.writer) {
    std::shared_ptr<arrow::io::FileOutputStream> outfile;
    auto file_result = arrow::io::FileOutputStream::Open(catInfo.filePath.string());
    if (!file_result.ok()) {
      throw std::runtime_error("Failed to open file: " + catInfo.filePath.string());
    }
    outfile = file_result.ValueOrDie();

    parquet::WriterProperties::Builder builder;
    if (m_options.compression == "ZSTD") {
      builder.compression(parquet::Compression::ZSTD);
    } else if (m_options.compression == "SNAPPY") {
      builder.compression(parquet::Compression::SNAPPY);
    } else if (m_options.compression == "UNCOMPRESSED") {
      builder.compression(parquet::Compression::UNCOMPRESSED);
    } else {
      throw std::invalid_argument("Unknown compression: " + m_options.compression);
    }

    auto arrow_props = parquet::ArrowWriterProperties::Builder().store_schema()->build();
    auto writer_result = parquet::arrow::FileWriter::Open(*catInfo.schema, arrow::default_memory_pool(), outfile,
                                                          builder.build(), arrow_props);
    if (!writer_result.ok()) {
      throw std::runtime_error("Failed to open parquet writer: " + writer_result.status().ToString());
    }
    catInfo.writer = std::move(writer_result.ValueOrDie());
  }

  auto status = catInfo.writer->WriteTable(*table, table->num_rows());
  if (!status.ok()) {
    throw std::runtime_error("Failed to write table to parquet: " + status.ToString());
  }
  catInfo.buffer.clear();
}

void ArrowWriter::finish() {
  if (m_finished) {
    return;
  }

  for (auto& [name, catInfo] : m_categories) {
    flushCategory(catInfo);
    if (catInfo.writer) {
      auto status = catInfo.writer->Close();
      if (!status.ok()) {
        throw std::runtime_error("Failed to close parquet writer: " + status.ToString());
      }
    }
  }

  writeMetadata();
  m_finished = true;
}

void ArrowWriter::writeMetadata() {
  nlohmann::json metadata;
  metadata["format"] = "podio-arrow";
  metadata["format_version"] = 1;
  metadata["podio_version"] = std::string(podio::version::build_version);

  nlohmann::json categoriesJson;
  for (const auto& [name, catInfo] : m_categories) {
    nlohmann::json catJson;
    catJson["file"] = catInfo.filePath.filename().string();
    catJson["entries"] = catInfo.entries;

    nlohmann::json collectionsJson = nlohmann::json::array();
    for (size_t i = 0; i < catInfo.collsToWrite.size(); ++i) {
      nlohmann::json collJson;
      collJson["name"] = catInfo.collsToWrite[i];
      collJson["value_type"] = catInfo.collTypes[i];
      collJson["schema_version"] = catInfo.collSchemaVersions[i];
      collJson["is_subset"] = catInfo.collIsSubset[i];
      collJson["id"] = catInfo.collIDs[i];
      collectionsJson.push_back(collJson);
    }
    catJson["collections"] = collectionsJson;
    categoriesJson[name] = catJson;
  }
  metadata["categories"] = categoriesJson;
  nlohmann::json edmDefsJson = nlohmann::json::object();
  nlohmann::json edmVersionsJson = nlohmann::json::object();
  for (const auto& [name, def] : m_datamodelCollector.getDatamodelDefinitionsToWrite()) {
    edmDefsJson[name] = def;
    auto edmVersion = podio::DatamodelRegistry::instance().getDatamodelVersion(name);
    if (edmVersion) {
      edmVersionsJson[name] = {{"major", edmVersion.value().major},
                               {"minor", edmVersion.value().minor},
                               {"patch", edmVersion.value().patch}};
    }
  }
  metadata["datamodel_definitions"] = edmDefsJson;
  metadata["datamodel_versions"] = edmVersionsJson;

  auto tmpPath = m_directory / "metadata.json.tmp";
  auto finalPath = m_directory / "metadata.json";

  std::ofstream out(tmpPath);
  if (!out) {
    throw std::runtime_error("Failed to open metadata.json.tmp for writing");
  }
  out << metadata.dump(2);
  out.close();

  std::filesystem::rename(tmpPath, finalPath);
}

} // namespace podio
