#include "podio/ArrowReader.h"
#include "podio/podioVersion.h"
#include "podio/utilities/DatamodelRegistryIOHelpers.h"

#include <arrow/io/file.h>
#include <arrow/result.h>
#include <arrow/status.h>
#include <arrow/table.h>
#include <arrow/type.h>
#include <arrow/util/config.h>
#include <nlohmann/json.hpp>
#include <parquet/arrow/reader.h>
#include <parquet/arrow/schema.h>

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <optional>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <variant>
#include <vector>

namespace podio {

namespace {

  void collectLeafIndices(const parquet::arrow::SchemaField& schemaField, std::vector<int>& colIndices) {
    if (schemaField.is_leaf()) {
      colIndices.push_back(schemaField.column_index);
    } else {
      for (const auto& child : schemaField.children) {
        collectLeafIndices(child, colIndices);
      }
    }
  }

} // namespace

ArrowReader::ArrowReader() = default;

void ArrowReader::openFile(const std::string& directory) {
  m_directory = directory;

  if (!std::filesystem::exists(m_directory) || !std::filesystem::is_directory(m_directory)) {
    throw std::runtime_error("Directory does not exist: " + directory);
  }

  auto metadataPath = std::filesystem::path(m_directory) / "metadata.json";
  if (!std::filesystem::exists(metadataPath)) {
    throw std::runtime_error("Missing metadata.json in directory: " + directory);
  }

  std::ifstream in(metadataPath);
  nlohmann::json metadata;
  in >> metadata;

  if (metadata.value("format", "") != "podio-arrow") {
    throw std::runtime_error("Unsupported format in metadata.json: " + metadata.value("format", ""));
  }
  if (metadata.value("format_version", 0) != 1) {
    throw std::runtime_error("Unsupported format_version in metadata.json: " +
                             std::to_string(metadata.value("format_version", 0)));
  }

  auto versionStr = metadata.value("podio_version", "");
  auto parsedVersion = podio::version::Version::fromString(versionStr);
  if (parsedVersion) {
    m_fileVersion = parsedVersion.value();
  } else {
    throw std::runtime_error("Invalid or missing podio_version in metadata.json: " + versionStr);
  }

  for (auto& [name, catJson] : metadata["categories"].items()) {
    CategoryInfo catInfo;
    catInfo.filePath = (std::filesystem::path(m_directory) / catJson["file"].get<std::string>()).string();
    catInfo.entries = catJson["entries"].get<size_t>();

    m_categories[name] = std::move(catInfo);
    m_availableCategories.push_back(name);
  }

  std::vector<std::tuple<std::string, std::string>> defs;
  std::vector<std::tuple<std::string, podio::version::Version>> versions;

  if (metadata.contains("datamodel_definitions")) {
    for (auto& [name, def] : metadata["datamodel_definitions"].items()) {
      defs.emplace_back(name, def.get<std::string>());
    }
  }

  if (metadata.contains("datamodel_versions")) {
    for (auto& [name, versionJson] : metadata["datamodel_versions"].items()) {
      versions.emplace_back(name,
                            podio::version::Version{versionJson["major"].get<uint16_t>(),
                                                    versionJson["minor"].get<uint16_t>(),
                                                    versionJson["patch"].get<uint16_t>()});
    }
  }

  m_datamodelHolder = DatamodelDefinitionHolder(std::move(defs), std::move(versions));
}

void ArrowReader::loadCategoryTable(CategoryInfo& catInfo, const std::vector<std::string>& collsToRead) {
  if (catInfo.allColumnsLoaded) {
    return;
  }

  if (catInfo.table && !collsToRead.empty()) {
    bool allRequestedPresent = true;
    for (const auto& collName : collsToRead) {
      if (catInfo.table->schema()->GetFieldIndex(collName) == -1) {
        allRequestedPresent = false;
        break;
      }
    }
    if (allRequestedPresent) {
      return;
    }
  }

  if (!std::filesystem::exists(catInfo.filePath)) {
    throw std::runtime_error("Missing category file: " + catInfo.filePath);
  }

  std::shared_ptr<arrow::io::ReadableFile> infile;
  auto file_result = arrow::io::ReadableFile::Open(catInfo.filePath);
  if (!file_result.ok()) {
    throw std::runtime_error("Failed to open file: " + file_result.status().ToString());
  }
  infile = file_result.ValueOrDie();

  std::unique_ptr<parquet::arrow::FileReader> reader;
#if ARROW_VERSION_MAJOR >= 19
  auto reader_result = parquet::arrow::OpenFile(infile, arrow::default_memory_pool());
  if (!reader_result.ok()) {
    throw std::runtime_error("Failed to open parquet reader: " + reader_result.status().ToString());
  }
  reader = std::move(reader_result.ValueOrDie());
#else
  auto reader_status = parquet::arrow::OpenFile(infile, arrow::default_memory_pool(), &reader);
  if (!reader_status.ok()) {
    throw std::runtime_error("Failed to open parquet reader: " + reader_status.ToString());
  }
#endif

  if (!catInfo.fullSchema) {
    auto schemaStatus = reader->GetSchema(&catInfo.fullSchema);
    if (!schemaStatus.ok()) {
      throw std::runtime_error("Failed to get schema from parquet reader: " + schemaStatus.ToString());
    }
  }

  for (const auto& collName : collsToRead) {
    if (catInfo.fullSchema->GetFieldIndex(collName) == -1) {
      throw std::invalid_argument(collName + " is not available from Frame");
    }
  }

  std::vector<std::string> targetFields;
  if (collsToRead.empty()) {
    for (int i = 0; i < catInfo.fullSchema->num_fields(); ++i) {
      targetFields.emplace_back(catInfo.fullSchema->field(i)->name());
    }
  } else {
    for (const auto& collName : collsToRead) {
      targetFields.push_back(collName);
    }
    if (catInfo.fullSchema->GetFieldIndex("frame_parameters") != -1) {
      targetFields.emplace_back("frame_parameters");
    }
  }

  std::vector<std::string> missingFields;
  if (!catInfo.table) {
    missingFields = std::move(targetFields);
  } else {
    for (auto& name : targetFields) {
      if (catInfo.table->schema()->GetFieldIndex(name) == -1) {
        missingFields.push_back(std::move(name));
      }
    }
  }

  std::ranges::sort(missingFields);
  const auto [mfFirst, mfLast] = std::ranges::unique(missingFields);
  missingFields.erase(mfFirst, mfLast);

  if (missingFields.empty()) {
    if (catInfo.table && catInfo.table->num_columns() == catInfo.fullSchema->num_fields()) {
      catInfo.allColumnsLoaded = true;
    }
    return;
  }

  if (!catInfo.table && missingFields.size() == static_cast<size_t>(catInfo.fullSchema->num_fields())) {
#if ARROW_VERSION_MAJOR >= 24
    auto result = reader->ReadTable();
    if (!result.ok()) {
      throw std::runtime_error("Failed to read arrow table: " + result.status().ToString());
    }
    catInfo.table = std::move(result.ValueOrDie());
#else
    std::shared_ptr<arrow::Table> table;
    auto status = reader->ReadTable(&table);
    if (!status.ok()) {
      throw std::runtime_error("Failed to read arrow table: " + status.ToString());
    }
    catInfo.table = std::move(table);
#endif
    catInfo.allColumnsLoaded = true;
  } else {
    const auto& manifest = reader->manifest();
    std::vector<int> colIndices;
    for (const auto& fieldName : missingFields) {
      for (const auto& schemaField : manifest.schema_fields) {
        if (schemaField.field && schemaField.field->name() == fieldName) {
          collectLeafIndices(schemaField, colIndices);
          break;
        }
      }
    }

    std::ranges::sort(colIndices);
    const auto [first, last] = std::ranges::unique(colIndices);
    colIndices.erase(first, last);

    std::shared_ptr<arrow::Table> newTable;
#if ARROW_VERSION_MAJOR >= 24
    auto result = reader->ReadTable(colIndices);
    if (!result.ok()) {
      throw std::runtime_error("Failed to read arrow table: " + result.status().ToString());
    }
    newTable = std::move(result.ValueOrDie());
#else
    auto status = reader->ReadTable(colIndices, &newTable);
    if (!status.ok()) {
      throw std::runtime_error("Failed to read arrow table: " + status.ToString());
    }
#endif

    if (!catInfo.table) {
      catInfo.table = std::move(newTable);
    } else {
      std::vector<std::shared_ptr<arrow::Field>> allFields = catInfo.table->schema()->fields();
      std::vector<std::shared_ptr<arrow::ChunkedArray>> allColumns = catInfo.table->columns();
      for (int i = 0; i < newTable->num_columns(); ++i) {
        allFields.push_back(newTable->schema()->field(i));
        allColumns.push_back(newTable->column(i));
      }
      catInfo.table = arrow::Table::Make(
          std::make_shared<arrow::Schema>(std::move(allFields), catInfo.fullSchema->metadata()), std::move(allColumns));
    }

    if (catInfo.table->num_columns() == catInfo.fullSchema->num_fields()) {
      catInfo.allColumnsLoaded = true;
    }
  }
}

std::unique_ptr<podio::ArrowFrameData> ArrowReader::readNextEntry(std::string_view name,
                                                                  const std::vector<std::string>& collsToRead) {
  auto it = m_categories.find(std::string(name));
  if (it == m_categories.end()) {
    return nullptr;
  }

  if (it->second.currentIndex >= it->second.entries) {
    return nullptr;
  }

  return readEntry(name, it->second.currentIndex, collsToRead);
}

std::unique_ptr<podio::ArrowFrameData> ArrowReader::readEntry(std::string_view name, size_t index,
                                                              const std::vector<std::string>& collsToRead) {
  auto it = m_categories.find(std::string(name));
  if (it == m_categories.end()) {
    return nullptr;
  }

  if (index >= it->second.entries) {
    return nullptr;
  }

  it->second.currentIndex = index + 1;
  loadCategoryTable(it->second, collsToRead);

  if (!collsToRead.empty()) {
    for (const auto& collName : collsToRead) {
      if (it->second.table->schema()->GetFieldIndex(collName) == -1) {
        throw std::invalid_argument(collName + " is not available from Frame");
      }
    }
  }

  return std::make_unique<podio::ArrowFrameData>(it->second.table, index, collsToRead, it->second.fullSchema);
}

size_t ArrowReader::getEntries(std::string_view name) const {
  auto it = m_categories.find(std::string(name));
  if (it != m_categories.end()) {
    return it->second.entries;
  }
  return 0;
}

} // namespace podio
