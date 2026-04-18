#ifndef PODIO_RNTUPLELAZYCATEGORYSTATE_H
#define PODIO_RNTUPLELAZYCATEGORYSTATE_H

#include "podio/CollectionIDTable.h"
#include "podio/utilities/RNTupleHelpers.h"

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

namespace podio {

/// Shared state between RNTupleLazyReader and RNTupleLazyFrameData instances
/// for the same category. Holds the full RNTuple readers, entry offset
/// information, collection metadata, and a cache of partial readers used for
/// per-collection lazy loading. The mutex serializes all RNTuple I/O operations
/// since ROOT is not thread-safe.
struct RNTupleCategoryState {
  RNTupleCategoryState() = default;
  ~RNTupleCategoryState() = default;
  RNTupleCategoryState(const RNTupleCategoryState&) = delete;
  RNTupleCategoryState& operator=(const RNTupleCategoryState&) = delete;
  RNTupleCategoryState(RNTupleCategoryState&&) = delete;
  RNTupleCategoryState& operator=(RNTupleCategoryState&&) = delete;

  /// The category name (needed when opening partial readers)
  std::string category{};

  /// Full RNTuple readers, one per file
  std::vector<std::unique_ptr<root_compat::RNTupleReader>> readers{};
  /// Filenames parallel to readers (needed to open partial readers for the same file)
  std::vector<std::string> filenames{};

  /// Cumulative entry offsets across readers. Entry i belongs to the reader
  /// whose index corresponds to the last value in this vector that is <= i.
  std::vector<unsigned> readerEntries{};
  unsigned totalEntries{0};

  /// Collection metadata read from the file metadata RNTuple
  std::vector<root_utils::CollectionWriteInfo> collectionInfo{};
  /// Shared collection ID table
  std::shared_ptr<const podio::CollectionIDTable> idTable{nullptr};

  /// Sequential entry counter used by readNextEntry()
  unsigned entry{0};

  /// Cache of partial readers, keyed by (readerIndex, collectionName).
  /// Each partial reader has a minimal RNTuple model with only the fields
  /// needed for one collection, so LoadEntry() only reads that collection's data.
  std::map<std::pair<size_t, std::string>, std::unique_ptr<root_compat::RNTupleReader>> partialReaders{};

  /// Mutex serializing all RNTuple I/O operations and partial reader creation
  std::mutex mutex{};
};

} // namespace podio

#endif // PODIO_RNTUPLELAZYCATEGORYSTATE_H
