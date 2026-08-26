#ifndef PODIO_ARROWREADER_H
#define PODIO_ARROWREADER_H

#include "podio/podioVersion.h"
#include "podio/utilities/ArrowFrameData.h"
#include "podio/utilities/ReaderCommon.h"

#include <cstddef>
#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace arrow {
class Table;
}

namespace podio {

/// Arrow backend reader for PODIO
///
/// Reads data from a directory structure containing one Parquet file per category
/// and a metadata.json file containing metadata for reading.
class ArrowReader : public ReaderCommon {
public:
  /// Create an ArrowReader
  ArrowReader();

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
  std::unique_ptr<podio::ArrowFrameData> readNextEntry(std::string_view name,
                                                       const std::vector<std::string>& collsToRead = {});

  /// Read the specific entry for the given category
  std::unique_ptr<podio::ArrowFrameData> readEntry(std::string_view name, size_t index,
                                                   const std::vector<std::string>& collsToRead = {});

  /// Get the number of entries for a category
  size_t getEntries(std::string_view name) const;

private:
  struct CategoryInfo {
    std::string filePath{};
    size_t entries = 0;
    size_t currentIndex = 0;
    std::shared_ptr<arrow::Table> table{nullptr};
  };

  void loadCategoryTable(CategoryInfo& catInfo);

  std::string m_directory{};
  std::map<std::string, CategoryInfo> m_categories{};
};

} // namespace podio

#endif // PODIO_ARROWREADER_H
