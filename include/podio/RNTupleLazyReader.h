#ifndef PODIO_RNTUPLELAZY_READER_H
#define PODIO_RNTUPLELAZY_READER_H

#include "podio/RNTupleLazyFrameData.h"
#include "podio/podioVersion.h"
#include "podio/utilities/ReaderCommon.h"

#include <ROOT/RNTuple.hxx>
#include <ROOT/RNTupleReader.hxx>
#include <RVersion.h>

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace podio {

namespace root_compat {
#if ROOT_VERSION_CODE < ROOT_VERSION(6, 35, 0)
  using RNTupleReader = ROOT::Experimental::RNTupleReader;
#else
  using RNTupleReader = ROOT::RNTupleReader;
#endif
} // namespace root_compat

struct RNTupleCategoryState;

/// The RNTupleLazyReader reads files written with the RNTuple backend lazily:
/// individual collections are only read from disk when they are first accessed
/// via Frame::get().
///
/// It provides data as RNTupleLazyFrameData from which a podio::Frame can be
/// constructed. Unlike RNTupleReader which reads all collections eagerly, this
/// reader defers the actual RNTuple I/O to the point of collection access,
/// using partial RNTuple readers with minimal field models so that LoadEntry()
/// only reads the requested collection's data.
class RNTupleLazyReader : public ReaderCommon {

public:
  RNTupleLazyReader() = default;
  ~RNTupleLazyReader() = default;

  RNTupleLazyReader(const RNTupleLazyReader&) = delete;
  RNTupleLazyReader& operator=(const RNTupleLazyReader&) = delete;
  RNTupleLazyReader(RNTupleLazyReader&&) = default;
  RNTupleLazyReader& operator=(RNTupleLazyReader&&) = default;

  /// Open a single file for reading.
  ///
  /// @param filename The name of the input file
  void openFile(const std::string& filename);

  /// Open multiple files for reading and treat them as if they are one file.
  ///
  /// @param filenames The filenames of all input files that should be read
  void openFiles(const std::vector<std::string>& filenames);

  /// Read the next data entry for a given category.
  ///
  /// @param name       The category name for which to read the next entry
  /// @param collsToRead (optional) the collection names that should be
  ///                   available for lazy reading. If not provided (or empty)
  ///                   all collections will be available.
  ///
  /// @returns FrameData from which a podio::Frame can be constructed if the
  ///          category exists and there are still entries left. Otherwise nullptr.
  ///          No collection data is read at this point.
  ///
  /// @throws std::invalid_argument if collsToRead contains collection names
  ///         that are not available
  std::unique_ptr<podio::RNTupleLazyFrameData> readNextEntry(std::string_view name,
                                                             const std::vector<std::string>& collsToRead = {});

  /// Read the desired data entry for a given category.
  ///
  /// @param name       The category name for which to read the next entry
  /// @param entry      The entry number to read
  /// @param collsToRead (optional) the collection names that should be
  ///                   available for lazy reading. If not provided (or empty)
  ///                   all collections will be available.
  ///
  /// @returns FrameData from which a podio::Frame can be constructed if the
  ///          category and the desired entry exist. Otherwise nullptr.
  ///          No collection data is read at this point.
  ///
  /// @throws std::invalid_argument if collsToRead contains collection names
  ///         that are not available
  std::unique_ptr<podio::RNTupleLazyFrameData> readEntry(std::string_view name, unsigned entry,
                                                         const std::vector<std::string>& collsToRead = {});

  /// Get the number of entries for the given category.
  unsigned getEntries(std::string_view name) const;

  /// Get the names of all the available Frame categories in the current file(s).
  std::vector<std::string_view> getAvailableCategories() const;

private:
  /// Initialize the given category: read collection info and ID table from the
  /// metadata RNTuple into the category state.
  bool initCategory(std::string_view category);

  /// Read and reconstruct the generic parameters of the Frame from the given
  /// reader at the given local entry index.
  GenericParameters readEventMetaData(root_compat::RNTupleReader* reader, unsigned localEntry);

  std::unique_ptr<root_compat::RNTupleReader> m_metadata{};

  std::unordered_map<std::string, std::unique_ptr<root_compat::RNTupleReader>> m_metadata_readers{};
  std::vector<std::string> m_filenames{};
  std::vector<std::string> m_availableCategories{};

  /// Per-category shared state (readers, entry offsets, collection info, etc.)
  std::unordered_map<std::string_view, std::shared_ptr<RNTupleCategoryState>> m_categoryStates{};
};

} // namespace podio

#endif // PODIO_RNTUPLELAZY_READER_H
