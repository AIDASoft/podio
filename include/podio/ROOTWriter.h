#ifndef PODIO_ROOTWRITER_H
#define PODIO_ROOTWRITER_H

#include "podio/CollectionBranches.h"
#include "podio/CollectionIDTable.h"
#include "podio/utilities/DatamodelRegistryIOHelpers.h"

#include "TFile.h"

#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

// forward declarations
class TTree;

namespace podio {
class Frame;
class CollectionBase;
class GenericParameters;

class ROOTWriter {
public:
  ROOTWriter(const std::string& filename);
  ~ROOTWriter();

  ROOTWriter(const ROOTWriter&) = delete;
  ROOTWriter& operator=(const ROOTWriter&) = delete;

  /** Store the given frame with the given category. Store all available
   * collections from the Frame.
   *
   * NOTE: The contents of the first Frame that is written in this way
   * determines the contents that will be written for all subsequent Frames.
   */
  void writeFrame(const podio::Frame& frame, const std::string& category);

  /** Store the given Frame with the given category. Store only the
   * collections that are passed.
   *
   * NOTE: The contents of the first Frame that is written in this way
   * determines the contents that will be written for all subsequent Frames.
   */
  void writeFrame(const podio::Frame& frame, const std::string& category, const std::vector<std::string>& collsToWrite);

  /** Write the current file, including all the necessary metadata to read it again.
   */
  void finish();

  /** Check whether the collsToWrite are consistent with the state of the passed
   * category.
   *
   * Return two vectors of collection names. The first one contains all the
   * names that were missing from the collsToWrite but were present in the
   * category. The second one contains the names that are present in the
   * collsToWrite only. If both vectors are empty the category and the passed
   * collsToWrite are consistent.
   *
   * NOTE: This will only be a meaningful check if the first Frame of the passed
   * category has already been written. Also, this check is rather expensive as
   * it has to effectively do two set differences.
   */
  std::tuple<std::vector<std::string>, std::vector<std::string>>
  checkConsistency(const std::vector<std::string>& collsToWrite, const std::string& category) const;

private:
  using StoreCollection = std::pair<const std::string&, podio::CollectionBase*>;

  // collectionID, collectionType, subsetCollection
  // NOTE: same as in rootUtils.h private header!
  using CollectionInfoT = std::tuple<uint32_t, std::string, bool, unsigned int>;

  /**
   * Helper struct to group together all necessary state to write / process a
   * given category. Created during the first writing of a category
   */
  struct CategoryInfo {
    TTree* tree{nullptr};                                   ///< The TTree to which this category is written
    std::vector<root_utils::CollectionBranches> branches{}; ///< The branches for this category
    std::vector<CollectionInfoT> collInfo{};                ///< Collection info for this category
    podio::CollectionIDTable idTable{};                     ///< The collection id table for this category
    std::vector<std::string> collsToWrite{};                ///< The collections to write for this category
  };

  /// Initialize the branches for this category
  void initBranches(CategoryInfo& catInfo, const std::vector<StoreCollection>& collections,
                    /*const*/ podio::GenericParameters& parameters);

  /// Get the (potentially uninitialized category information for this category)
  CategoryInfo& getCategoryInfo(const std::string& category);

  static void resetBranches(std::vector<root_utils::CollectionBranches>& branches,
                            const std::vector<ROOTWriter::StoreCollection>& collections,
                            /*const*/ podio::GenericParameters* parameters);

  std::unique_ptr<TFile> m_file{nullptr};                       ///< The storage file
  std::unordered_map<std::string, CategoryInfo> m_categories{}; ///< All categories

  DatamodelDefinitionCollector m_datamodelCollector{};

  bool m_finished{false}; ///< Whether writing has been actually done
};

} // namespace podio

#endif // PODIO_ROOTWRITER_H
