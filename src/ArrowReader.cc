#include "podio/ArrowReader.h"

#include <arrow/io/file.h>
#include <arrow/table.h>
#include <arrow/util/config.h>
#include <nlohmann/json.hpp>
#include <parquet/arrow/reader.h>

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace podio {

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

void ArrowReader::loadCategoryTable(CategoryInfo& catInfo) {
  if (catInfo.table) {
    return;
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

  return readEntry(name, it->second.currentIndex++, collsToRead);
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
  loadCategoryTable(it->second);

  if (!collsToRead.empty()) {
    for (const auto& collName : collsToRead) {
      if (it->second.table->schema()->GetFieldIndex(collName) == -1) {
        throw std::invalid_argument(collName + " is not available from Frame");
      }
    }
  }

  return std::make_unique<podio::ArrowFrameData>(it->second.table, index, collsToRead);
}

size_t ArrowReader::getEntries(std::string_view name) const {
  auto it = m_categories.find(std::string(name));
  if (it != m_categories.end()) {
    return it->second.entries;
  }
  return 0;
}

} // namespace podio
