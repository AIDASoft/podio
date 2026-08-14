#ifndef PODIO_ARROWWRITER_H
#define PODIO_ARROWWRITER_H

#include "podio/utilities/DatamodelRegistryIOHelpers.h"

#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <map>
#include <cstdint>
#include <cstddef>

// Forward declarations for Arrow and Parquet
namespace arrow {
class Schema;
class Table;
} // namespace arrow

namespace parquet::arrow {
class FileWriter;
} // namespace parquet::arrow

namespace podio {

class Frame;

/// Arrow backend writer for PODIO
///
/// Writes data to a directory structure containing one Parquet file per category
/// and a metadata.json file containing metadata for reading.
class ArrowWriter {
public:
  /// Configure the ArrowWriter
  struct Options {
    size_t maxBufferedRows = 1000;
    std::string compression = "ZSTD"; // ZSTD, SNAPPY, UNCOMPRESSED, etc.
  };

  /// Create a ArrowWriter to write to a directory.
  ///
  /// @note Will create the directory if it doesn't exist. Will throw if it exists
  ///       and is not empty.
  ///
  /// @param directory The path to the output directory.
  /// @param options   Configuration options for buffering and compression.
  ArrowWriter(const std::string& directory, Options options);
  explicit ArrowWriter(const std::string& directory);

  /// Destructor writes metadata and closes files.
  ~ArrowWriter();

  ArrowWriter(const ArrowWriter&) = delete;
  ArrowWriter& operator=(const ArrowWriter&) = delete;
  ArrowWriter(ArrowWriter&&) = delete;
  ArrowWriter& operator=(ArrowWriter&&) = delete;

  /// Store the given frame with the given category.
  void writeFrame(const podio::Frame& frame, std::string_view category);

  /// Store the given Frame with the given category, specifying collections.
  void writeFrame(const podio::Frame& frame, std::string_view category, const std::vector<std::string>& collsToWrite);

  /// Write the current directory including metadata.json and close files.
  void finish();

private:
  /// Helper struct to manage category state
  struct CategoryInfo {
    std::filesystem::path filePath;
    std::shared_ptr<arrow::Schema> schema;
    std::vector<std::string> collsToWrite;
    std::vector<std::string> collTypes;
    std::vector<bool> collIsSubset;
    std::vector<uint32_t> collSchemaVersions;
    std::vector<uint32_t> collIDs;
    std::vector<std::shared_ptr<arrow::Table>> buffer;
    std::unique_ptr<parquet::arrow::FileWriter> writer;
    size_t entries = 0;
  };

  void flushCategory(CategoryInfo& catInfo);
  void writeMetadata();
  void validateSchema(CategoryInfo& catInfo, const podio::Frame& frame, const std::vector<std::string>& collsToWrite);

  std::filesystem::path m_directory;
  Options m_options;
  std::map<std::string, CategoryInfo> m_categories;
  DatamodelDefinitionCollector m_datamodelCollector;
  bool m_finished = false;
};

} // namespace podio

#endif // PODIO_ARROWWRITER_H
