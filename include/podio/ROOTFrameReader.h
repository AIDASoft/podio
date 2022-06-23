#ifndef PODIO_ROOTFRAMEREADER_H
#define PODIO_ROOTFRAMEREADER_H

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
 * This class has the function to read available data from disk
 * and to prepare collections and buffers.
 **/
class ROOTFrameReader {

public:
  ROOTFrameReader() = default;
  ~ROOTFrameReader() = default;

  // non-copyable
  ROOTFrameReader(const ROOTFrameReader&) = delete;
  ROOTFrameReader& operator=(const ROOTFrameReader&) = delete;

  void openFile(const std::string& filename);

  void openFiles(const std::vector<std::string>& filenames);

  /**
   * Read the next data entry from which a Frame can be constructed for the
   * given name. In case there are no more entries left for this name or in
   * case there is no data for this name, this returns a nullptr.
   */
  std::unique_ptr<podio::ROOTFrameData> readNextEntry(const std::string& name);

  /// Returns number of entries for the given name
  unsigned getEntries(const std::string& name) const;

  podio::version::Version currentFileVersion() const {
    return m_fileVersion;
  }

private:
  /**
   * Helper struct to group together all the necessary state to read / process a
   * given category. A "category" in this case describes all frames with the
   * same name which are constrained by the ROOT file structure that we use to
   * have the same contents. It encapsulates all state that is necessary for
   * reading from a TTree / TChain (i.e. collection infos, branches, ...)
   */
  struct CategoryInfo {
    /// constructor from chain for more convenient map instertion
    CategoryInfo(std::unique_ptr<TChain>&& c) : chain(std::move(c)) {
    }
    std::unique_ptr<TChain> chain{nullptr};                                      ///< The TChain with the data
    unsigned entry{0};                                                           ///< The next entry to read
    std::vector<std::pair<std::string, detail::CollectionInfo>> storedClasses{}; ///< The stored collections in this
                                                                                 ///< category
    std::vector<root_utils::CollectionBranches> branches{};                      ///< The branches for this category
    std::shared_ptr<CollectionIDTable> table{nullptr}; ///< The collection ID table for this category
  };

  /**
   * Initialze the passed CategoryInfo by setting up the necessary branches,
   * collection infos and all necessary meta data to be able to read entries
   * with this name
   */
  void initCategory(CategoryInfo& catInfo, const std::string& name);

  /**
   * Get the category information for the given name. In case there is no TTree
   * with contents for the given name this will return a CategoryInfo with an
   * uninitialized chain (nullptr) member
   */
  CategoryInfo& getCategoryInfo(const std::string& name);

  GenericParameters readEventMetaData(CategoryInfo& catInfo);

  /**
   * Get / read the buffers at index iColl in the passed category information
   */
  podio::CollectionReadBuffers getCollectionBuffers(CategoryInfo& catInfo, size_t iColl);

  std::unique_ptr<TChain> m_metaChain{nullptr};                 ///< The metadata tree
  std::unordered_map<std::string, CategoryInfo> m_categories{}; ///< All categories
  std::vector<std::string> m_availCategories{};                 ///< All available categories from this file

  podio::version::Version m_fileVersion{0, 0, 0};
};

} // namespace podio

#endif // PODIO_ROOTFRAMEREADER_H
