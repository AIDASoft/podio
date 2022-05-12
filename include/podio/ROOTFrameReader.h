#ifndef PODIO_ROOTFRAMEREADER_H
#define PODIO_ROOTFRAMEREADER_H

#include "podio/CollectionBranches.h"
#include "podio/ROOTRawData.h"
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

  /// Read all collections requested
  std::unique_ptr<podio::ROOTRawData> readNextEvent(const std::string& category);

  /// Returns number of entries for the given category
  unsigned getEntries(const std::string& category) const;

  podio::version::Version currentFileVersion() const {
    return m_fileVersion;
  }

  // Information about the data vector as wall as the collection class type
  // and the index in the collection branches cache vector
  // TODO: Make this private again
  using CollectionInfo = std::tuple<const TClass*, const TClass*, size_t>;

private:
  /**
   * Helper struct to group together all the necessary state to read / process a
   * given category.
   */
  struct CategoryInfo {
    /// constructor from chain for more convenient map instertion
    CategoryInfo(std::unique_ptr<TChain>&& c) : chain(std::move(c)) {
    }
    std::unique_ptr<TChain> chain{nullptr};                              ///< The TChain with the data
    unsigned entry{0};                                                   ///< The next entry to read
    std::vector<std::pair<std::string, CollectionInfo>> storedClasses{}; ///< The stored collections in this category
    std::vector<root_utils::CollectionBranches> branches{};              ///< The branches for this category
    std::shared_ptr<CollectionIDTable> table{nullptr};                   ///< The collection ID table for this category
  };

  void initCategory(CategoryInfo& catInfo, const std::string& category);

  CategoryInfo& getCategoryInfo(const std::string& category);

  GenericParameters readEventMetaData(CategoryInfo& catInfo);

  podio::CollectionReadBuffers getCollectionBuffers(CategoryInfo& catInfo, size_t iColl);

  std::unique_ptr<TChain> m_metaChain{nullptr};                 ///< The metadata tree
  std::unordered_map<std::string, CategoryInfo> m_categories{}; ///< All categories
  std::vector<std::string> m_availCategories{};                 ///< All available categories from this file

  podio::version::Version m_fileVersion{0, 0, 0};
};

} // namespace podio

#endif // PODIO_ROOTFRAMEREADER_H
