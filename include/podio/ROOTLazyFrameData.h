#ifndef PODIO_ROOTLAZYFRAMEDATA_H
#define PODIO_ROOTLAZYFRAMEDATA_H

#include "podio/CollectionBuffers.h"
#include "podio/CollectionIDTable.h"
#include "podio/GenericParameters.h"

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace podio {

struct CategoryState;

/// FrameData implementation for lazy ROOT reading. Instead of holding
/// pre-populated buffers, it holds a reference to the shared CategoryState and
/// reads individual collections from disk on demand when getCollectionBuffers()
/// is called.
class ROOTLazyFrameData {
  using CollIDPtr = std::shared_ptr<const podio::CollectionIDTable>;

public:
  ROOTLazyFrameData() = delete;
  ~ROOTLazyFrameData() = default;
  ROOTLazyFrameData(ROOTLazyFrameData&&) = default;
  ROOTLazyFrameData& operator=(ROOTLazyFrameData&&) = default;
  ROOTLazyFrameData(const ROOTLazyFrameData&) = delete;
  ROOTLazyFrameData& operator=(const ROOTLazyFrameData&) = delete;

  /// Construct from shared state, entry number, available collections, ID table
  /// and eagerly-read parameters
  ROOTLazyFrameData(std::shared_ptr<CategoryState> state, unsigned entry,
                    std::unordered_map<std::string, size_t>&& availableCollections, CollIDPtr idTable,
                    podio::GenericParameters&& params);

  /// Lazily read a single collection from ROOT. Each call reads exactly one
  /// collection from disk, positions the TChain, refreshes branch pointers,
  /// and returns the populated buffers.
  std::optional<podio::CollectionReadBuffers> getCollectionBuffers(const std::string& name);

  podio::CollectionIDTable getIDTable() const;

  std::unique_ptr<podio::GenericParameters> getParameters();

  std::vector<std::string> getAvailableCollections() const;

private:
  std::shared_ptr<CategoryState> m_state;
  unsigned m_entry{0};
  /// Maps collection name to its index in CategoryState::storedClasses.
  /// Collections are removed from this map once extracted.
  std::unordered_map<std::string, size_t> m_availableCollections{};
  CollIDPtr m_idTable{nullptr};
  podio::GenericParameters m_parameters{};
};

} // namespace podio

#endif // PODIO_ROOTLAZYFRAMEDATA_H
