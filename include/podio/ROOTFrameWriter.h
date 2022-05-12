#ifndef PODIO_ROOTFRAMEWRITER_H
#define PODIO_ROOTFRAMEWRITER_H

#include "podio/CollectionBranches.h"
#include "podio/CollectionIDTable.h"

#include "TFile.h"

#include <memory>
#include <string>
#include <tuple>
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

  void writeFrame(const podio::Frame& frame);

  /// Register a collection to be written (without check if it is actually present!)
  void registerForWrite(const std::string& name);

  void finish();

private:
  using StoreCollection = std::pair<const std::string&, podio::CollectionBase*>;

  std::tuple<TTree*, std::vector<root_utils::CollectionBranches>>
  initTree(const std::vector<StoreCollection>& collections, /*const*/ podio::GenericParameters& parameters,
           podio::CollectionIDTable&& idTable, const std::string& category);

  static void resetBranches(std::vector<root_utils::CollectionBranches>& branches,
                            const std::vector<ROOTFrameWriter::StoreCollection>& collections,
                            /*const*/ podio::GenericParameters* parameters);

  std::unique_ptr<TFile> m_file{nullptr}; ///< The storage file
  TTree* m_dataTree{nullptr};             ///< Collection data and parameters for each frame

  /// The tree to store all the necessary metadata to read this file again
  TTree* m_metaTree{nullptr};

  std::vector<std::string> m_collsToWrite{}; ///< The collections that should be written

  /// Cached branches for writing collections
  std::vector<root_utils::CollectionBranches> m_collectionBranches{};

  podio::CollectionIDTable m_idTable{}; ///< The collection id table

  // collectionID, collectionType, subsetCollection
  // NOTE: same as in rootUtils.h private header!
  using CollectionInfoT = std::tuple<int, std::string, bool>;

  std::vector<CollectionInfoT> m_collectionInfo{};
};

} // namespace podio

#endif // PODIO_ROOTFRAMEWRITER_H
