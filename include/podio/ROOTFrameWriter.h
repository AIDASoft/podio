#ifndef PODIO_ROOTFRAMEWRITER_H
#define PODIO_ROOTFRAMEWRITER_H

#include "podio/CollectionBranches.h"
#include "podio/CollectionIDTable.h"

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

class ROOTFrameWriter {
public:
  ROOTFrameWriter(const std::string& filename);
  ~ROOTFrameWriter() = default;

  ROOTFrameWriter(const ROOTFrameWriter&) = delete;
  ROOTFrameWriter& operator=(const ROOTFrameWriter&) = delete;

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

private:
  using StoreCollection = std::pair<const std::string&, podio::CollectionBase*>;

  // collectionID, collectionType, subsetCollection
  // NOTE: same as in rootUtils.h private header!
  using CollectionInfoT = std::tuple<int, std::string, bool>;

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
                            const std::vector<ROOTFrameWriter::StoreCollection>& collections,
                            /*const*/ podio::GenericParameters* parameters);

  std::unique_ptr<TFile> m_file{nullptr};                       ///< The storage file
  std::unordered_map<std::string, CategoryInfo> m_categories{}; ///< All categories
};

} // namespace podio

#endif // PODIO_ROOTFRAMEWRITER_H
