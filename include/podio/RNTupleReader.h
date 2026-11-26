#ifndef PODIO_RNTUPLEREADER_H
#define PODIO_RNTUPLEREADER_H

#include "podio/GenericParameters.h"
#include "podio/ObjectID.h"
#include "podio/ROOTFrameData.h"
#include "podio/utilities/ReaderCommon.h"
#include "podio/utilities/RootHelpers.h"

#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include <ROOT/RNTuple.hxx>
#include <ROOT/RNTupleReader.hxx>
#include <ROOT/RNTupleView.hxx>

namespace podio {
class CollectionIDTable;

/// The RNTupleReader can be used to read files that have been written with the
/// RNTuple backend.
///
/// The RNTupleReader provides the data as ROOTFrameData from which a podio::Frame
/// can be constructed. It can be used to read files written by the RNTupleWriter.
/// Only fields of selected collections that can be bound to read buffers are
/// deserialized. Frame parameters are always read, independently of the selection.
///
/// The fields of a collection stay connected to the file once the collection
/// has been read from it, to avoid setting them up again for every entry. ROOT
/// fetches the data of all connected fields from storage, so after narrowing the
/// selection, the data of collections that are no longer requested is still
/// fetched, although it is not deserialized. Reading with a constant selection,
/// or using a separate reader for a different selection, avoids this.
class RNTupleReader : public ReaderCommon {

public:
  RNTupleReader() = default;
  ~RNTupleReader() = default;

  RNTupleReader(const RNTupleReader&) = delete;
  RNTupleReader& operator=(const RNTupleReader&) = delete;
  RNTupleReader(RNTupleReader&&) = default;
  RNTupleReader& operator=(RNTupleReader&&) = default;

  /// Open a single file for reading.
  ///
  /// @param filename The name of the input file
  ///
  /// @throws std::logic_error if files have already been opened with this reader
  void openFile(const std::string& filename);

  /// Open multiple files for reading and then treat them as if they are one file
  ///
  /// @note All of the files are assumed to have the same structure. Specifically
  /// this means:
  /// - The same categories are available from all files
  /// - The collections that are contained in the individual categories are the
  ///   same across all files
  /// - This usually boils down to "the files have been written with the same
  ///   "settings", e.g. they are outputs of a batched process.
  ///
  /// @param filenames The filenames of all input files that should be read
  ///
  /// @throws std::logic_error if files have already been opened with this reader
  void openFiles(const std::vector<std::string>& filenames);

  /// Read the next data entry for a given category.
  ///
  /// @param name The category name for which to read the next entry
  /// @param collsToRead (optional) the collection names that should be read. If
  ///             not provided (or empty) all collections will be read
  ///
  /// @returns FrameData from which a podio::Frame can be constructed if the
  ///          category exists and if there are still entries left to read.
  ///          Otherwise a nullptr
  ///
  /// @throws std::invalid_argument in case collsToRead contains collection
  /// names that are not available
  std::unique_ptr<podio::ROOTFrameData> readNextEntry(std::string_view name,
                                                      const std::vector<std::string>& collsToRead = {});

  /// Read the desired data entry for a given category.
  ///
  /// @param name  The category name for which to read the next entry
  /// @param entry The entry number to read
  /// @param collsToRead (optional) the collection names that should be read. If
  ///             not provided (or empty) all collections will be read
  ///
  /// @returns FrameData from which a podio::Frame can be constructed if the
  ///          category and the desired entry exist. Otherwise a nullptr
  ///
  /// @throws std::invalid_argument in case collsToRead contains collection
  /// names that are not available
  std::unique_ptr<podio::ROOTFrameData> readEntry(std::string_view name, const unsigned entry,
                                                  const std::vector<std::string>& collsToRead = {});

  /// Get the number of entries for the given name
  ///
  /// @param name The name of the category
  ///
  /// @returns The number of entries that are available for the category
  unsigned getEntries(std::string_view name) const;

private:
  /**
   * Initialize the given category by filling the maps with metadata information
   * that will be used later
   */
  bool initCategory(std::string_view category);

  template <typename T>
  struct ParameterViews {
    explicit ParameterViews(ROOT::RNTupleReader& reader);
    ROOT::RNTupleView<std::vector<std::string>> keys;
    ROOT::RNTupleView<std::vector<std::vector<T>>> values;
  };

  struct FrameParameterViews {
    explicit FrameParameterViews(ROOT::RNTupleReader& reader);
    ParameterViews<int> ints;
    ParameterViews<float> floats;
    ParameterViews<double> doubles;
    ParameterViews<std::string> strings;
  };

  /**
   * Read and reconstruct the generic parameters of the Frame
   */
  static GenericParameters readEventMetaData(FrameParameterViews& views, const unsigned localEntry);

  std::unique_ptr<ROOT::RNTupleReader> m_metadata{};

  struct CollectionViews {
    // The first data view is the collection data, followed by vector members.
    std::vector<ROOT::RNTupleView<void>> data{};
    // Keep the buffer index: relations absent from an older schema have no view.
    std::vector<std::pair<std::size_t, ROOT::RNTupleView<std::vector<podio::ObjectID>>>> references{};
  };

  struct ReaderWithViews {
    std::unique_ptr<ROOT::RNTupleReader> reader{};
    // Views borrow buffers only during readEntry and are rebound before use.
    // An empty optional marks a collection whose fields could not be bound, so
    // that the binding is not retried for every entry.
    std::unordered_map<std::string, std::optional<CollectionViews>> collectionViews{};
    // Created on first use, since views cannot be default constructed
    std::optional<FrameParameterViews> parameterViews{};
  };

  std::unordered_map<std::string_view, std::vector<ReaderWithViews>> m_readers{};
  std::unordered_map<std::string, std::unique_ptr<ROOT::RNTupleReader>> m_metadata_readers{};
  std::vector<std::string> m_filenames{};

  std::unordered_map<std::string_view, unsigned> m_entries{};
  // Map category to a vector that contains at how many entries each reader starts
  // For example, if we have 3 readers and the first one has 10 entries, the second one 20 and the third one 30
  // then the vector will be {0, 10, 30}
  // 60 is not needed because anything after 30 will be in the last reader
  std::unordered_map<std::string_view, std::vector<unsigned>> m_readerEntries{};
  std::unordered_map<std::string_view, unsigned> m_totalEntries{};

  /// Map each category to the collections that have been written and are available
  std::unordered_map<std::string_view, std::vector<podio::root_utils::CollectionWriteInfo>> m_collectionInfo{};

  std::unordered_map<std::string_view, std::shared_ptr<podio::CollectionIDTable>> m_idTables{};
};

} // namespace podio

#endif
