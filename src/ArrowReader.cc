#include "podio/ArrowReader.h"

#include <arrow/io/file.h>
#include <arrow/table.h>
#include <nlohmann/json.hpp>
#include <parquet/arrow/reader.h>

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace podio {

ArrowReader::ArrowReader() = default;

ArrowReader::ArrowReader(const std::string& directory) {
  openFile(directory);
}

void ArrowReader::openFile(const std::string& directory) {
  m_directory = directory;

  if (!std::filesystem::exists(m_directory) || !std::filesystem::is_directory(m_directory)) {
    throw std::runtime_error("Directory does not exist: " + directory);
  }

  auto metadataPath = m_directory / "metadata.json";
  if (!std::filesystem::exists(metadataPath)) {
    throw std::runtime_error("Missing metadata.json in directory: " + directory);
  }

  std::ifstream in(metadataPath);
  nlohmann::json metadata;
  in >> metadata;

  if (metadata.value("format", "") != "podio-arrow") {
    throw std::runtime_error("Unsupported format in metadata.json");
  }
  if (metadata.value("format_version", 0) != 1) {
    throw std::runtime_error("Unsupported format_version in metadata.json");
  }

  auto versionStr = metadata.value("podio_version", "");
  uint16_t major = 0, minor = 0, patch = 0;
  char dot1, dot2;
  std::stringstream ss(versionStr);
  if (ss >> major >> dot1 >> minor >> dot2 >> patch && dot1 == '.' && dot2 == '.') {
    m_fileVersion = podio::version::Version{major, minor, patch};
  } else {
    throw std::runtime_error("Invalid or missing podio_version in metadata.json");
  }

  for (auto& [name, catJson] : metadata["categories"].items()) {
    CategoryInfo catInfo;
    catInfo.filePath = m_directory / catJson["file"].get<std::string>();
    catInfo.entries = catJson["entries"].get<size_t>();

    m_categories[name] = std::move(catInfo);
    m_categoryNames.push_back(name);
  }

  for (const auto& name : m_categoryNames) {
    m_categoryViews.push_back(name);
  }

  if (metadata.contains("datamodel_definitions")) {
    for (auto& [name, def] : metadata["datamodel_definitions"].items()) {
      m_datamodelDefinitions[name] = def.get<std::string>();
      m_availableDatamodels.push_back(name);
    }
  }

  if (metadata.contains("datamodel_versions")) {
    for (auto& [name, versionJson] : metadata["datamodel_versions"].items()) {
      m_datamodelVersions[name] =
          podio::version::Version{versionJson["major"].get<uint16_t>(), versionJson["minor"].get<uint16_t>(),
                                  versionJson["patch"].get<uint16_t>()};
    }
  }
}

void ArrowReader::loadCategoryTable(CategoryInfo& catInfo) {
  if (catInfo.table) {
    return;
  }

  if (!std::filesystem::exists(catInfo.filePath)) {
    throw std::runtime_error("Missing category file: " + catInfo.filePath.string());
  }

  std::shared_ptr<arrow::io::ReadableFile> infile;
  auto file_result = arrow::io::ReadableFile::Open(catInfo.filePath.string());
  if (!file_result.ok()) {
    throw std::runtime_error("Failed to open file: " + file_result.status().ToString());
  }
  infile = file_result.ValueOrDie();

  auto reader_result = parquet::arrow::OpenFile(infile, arrow::default_memory_pool());
  if (!reader_result.ok()) {
    throw std::runtime_error("Failed to open parquet reader: " + reader_result.status().ToString());
  }
  std::unique_ptr<parquet::arrow::FileReader> reader = std::move(reader_result.ValueOrDie());

  std::shared_ptr<arrow::Table> table;
  auto status = reader->ReadTable(&table);
  if (!status.ok()) {
    throw std::runtime_error("Failed to read arrow table: " + status.ToString());
  }
  catInfo.table = std::move(table);
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

podio::version::Version ArrowReader::currentFileVersion() const {
  return m_fileVersion;
}

std::optional<podio::version::Version> ArrowReader::currentFileVersion(std::string_view name) const {
  auto it = m_datamodelVersions.find(std::string(name));
  if (it != m_datamodelVersions.end()) {
    return it->second;
  }
  return std::nullopt;
}

const std::vector<std::string_view>& ArrowReader::getAvailableCategories() const {
  return m_categoryViews;
}

const std::string_view ArrowReader::getDatamodelDefinition(std::string_view name) const {
  auto it = m_datamodelDefinitions.find(std::string(name));
  if (it != m_datamodelDefinitions.end()) {
    return it->second;
  }
  return "";
}

const std::vector<std::string>& ArrowReader::getAvailableDatamodels() const {
  return m_availableDatamodels;
}

} // namespace podio
