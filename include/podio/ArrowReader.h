#ifndef PODIO_ARROWREADER_H
#define PODIO_ARROWREADER_H

#include "podio/podioVersion.h"
#include "podio/utilities/ArrowFrameData.h"

#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <map>
#include <cstddef>
#include <optional>

namespace arrow {
class Table;
}

namespace podio {

/// Arrow backend reader for PODIO
///
/// Reads data from a directory structure containing one Parquet file per category
/// and a metadata.json file containing metadata for reading.
class ArrowReader {
public:
  /// Create an ArrowReader
  ArrowReader();

  /// Create a ArrowReader to read from a directory.
  ///
  /// @param directory The path to the dataset directory.
  ArrowReader(const std::string& directory);

  /// Open the passed directory for reading.
  ///
  /// @param directory The path to the directory to read from
  void openFile(const std::string& directory);

  ~ArrowReader() = default;

  ArrowReader(const ArrowReader&) = delete;
  ArrowReader& operator=(const ArrowReader&) = delete;
  ArrowReader(ArrowReader&&) = delete;
  ArrowReader& operator=(ArrowReader&&) = delete;

  /// Read the next entry for the given category
  std::unique_ptr<podio::ArrowFrameData> readNextEntry(std::string_view name, const std::vector<std::string>& collsToRead = {});

  /// Read the specific entry for the given category
  std::unique_ptr<podio::ArrowFrameData> readEntry(std::string_view name, size_t index, const std::vector<std::string>& collsToRead = {});

  /// Get the number of entries for a category
  size_t getEntries(std::string_view name) const;

  /// Get the podio build version
  podio::version::Version currentFileVersion() const;

  /// Get the datamodel build version
  std::optional<podio::version::Version> currentFileVersion(std::string_view name) const;

  /// Get available categories
  std::vector<std::string_view> getAvailableCategories() const;

  /// Get datamodel definition
  const std::string_view getDatamodelDefinition(std::string_view name) const;

  /// Get available datamodels
  std::vector<std::string> getAvailableDatamodels() const;

private:
  struct CategoryInfo {
    std::filesystem::path filePath;
    size_t entries = 0;
    size_t currentIndex = 0;
    std::shared_ptr<arrow::Table> table;
  };

  void loadCategoryTable(CategoryInfo& catInfo);

  std::filesystem::path m_directory;
  podio::version::Version m_fileVersion;
  
  std::map<std::string, CategoryInfo> m_categories;
  std::vector<std::string> m_categoryNames;
  std::vector<std::string_view> m_categoryViews;
  
  std::map<std::string, std::string> m_datamodelDefinitions;
  std::map<std::string, podio::version::Version> m_datamodelVersions;
  std::vector<std::string> m_availableDatamodels;
};

} // namespace podio

#endif // PODIO_ARROWREADER_H
