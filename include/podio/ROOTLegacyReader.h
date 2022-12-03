#ifndef PODIO_ROOTLEGACYREADER_H
#define PODIO_ROOTLEGACYREADER_H

#include "podio/CollectionBranches.h"
#include "podio/ROOTFrameData.h"
#include "podio/podioVersion.h"

#include "TChain.h"

#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

// forward declarations
class TClass;
// class TChain;
class TFile;
class TTree;

namespace podio {

namespace detail {
  // Information about the data vector as wall as the collection class type
  // and the index in the collection branches cache vector
  using CollectionInfo = std::tuple<const TClass*, const TClass*, size_t>;

} // namespace detail

class EventStore;
class CollectionBase;
class CollectionIDTable;
class GenericParameters;
struct CollectionReadBuffers;

/**
 * A root reader for reading legacy podio root files that have been written
 * using the legacy, non Frame based I/O model. This reader grants Frame based
 * access to those files, by mimicking the Frame I/O functionality and the
 * interfaces of those readers.
 *
 * NOTE: Since there was only one category ("events") for those legacy podio
 * files this reader will really only work if you try to read that category, and
 * will simply return no data if you try to read anything else.
 */
class ROOTLegacyReader {

public:
  ROOTLegacyReader() = default;
  ~ROOTLegacyReader() = default;

  // non-copyable
  ROOTLegacyReader(const ROOTLegacyReader&) = delete;
  ROOTLegacyReader& operator=(const ROOTLegacyReader&) = delete;

  bool openFile(const std::string& filename);

  bool openFiles(const std::vector<std::string>& filenames);

  /**
   * Read the next data entry from which a Frame can be constructed. In case
   * there are no more entries left, this returns a nullptr.
   *
   * NOTE: the category name has to be "events" in this case, as only that
   * category is available for legacy files.
   */
  std::unique_ptr<podio::ROOTFrameData> readNextEntry(const std::string&);

  /**
   * Read the specified data entry from which a Frame can be constructed In case
   * the entry does not exist, this returns a nullptr.
   *
   * NOTE: the category name has to be "events" in this case, as only that
   * category is available for legacy files.
   */
  std::unique_ptr<podio::ROOTFrameData> readEntry(const std::string&, const unsigned entry);

  /// Returns number of entries for a given category
  unsigned getEntries(const std::string&) const;

  /// Get the build version of podio that has been used to write the current file
  podio::version::Version currentFileVersion() const {
    return m_fileVersion;
  }

  /// Get the names of all the availalable Frame categories in the current file(s)
  std::vector<std::string_view> getAvailableCategories() const;

private:
  std::pair<TTree*, unsigned> getLocalTreeAndEntry(const std::string& treename);

  void createCollectionBranches(const std::vector<std::tuple<int, std::string, bool>>& collInfo);

  podio::GenericParameters readEventMetaData();

  podio::CollectionReadBuffers getCollectionBuffers(const std::pair<std::string, detail::CollectionInfo>& collInfo);

  std::unique_ptr<podio::ROOTFrameData> readEntry();

  // cache the necessary information to more quickly construct and read each
  // collection after it has been read the very first time
  std::vector<std::pair<std::string, detail::CollectionInfo>> m_storedClasses{};

  std::shared_ptr<CollectionIDTable> m_table{nullptr};
  std::unique_ptr<TChain> m_chain{nullptr};
  unsigned m_eventNumber{0};

  // Similar to writing we cache the branches that belong to each collection
  // in order to not having to look them up every event. However, for the
  // reader we cannot guarantee a fixed order of collections as they are read
  // on demand. Hence, we give each collection an index the first time it is
  // read and we start caching the branches.
  std::vector<root_utils::CollectionBranches> m_collectionBranches{};

  podio::version::Version m_fileVersion{0, 0, 0};

  /// The **only** category name that is available from legacy files
  constexpr static auto m_categoryName = "events";
};

} // namespace podio

#endif // PODIO_ROOTLEGACYREADER_H
