#ifndef PODIO_RNTUPLELAZY_FRAMEDATA_H
#define PODIO_RNTUPLELAZY_FRAMEDATA_H

#include "podio/CollectionBuffers.h"
#include "podio/CollectionIDTable.h"
#include "podio/GenericParameters.h"

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace podio {

struct RNTupleCategoryState;

/// FrameData implementation for lazy RNTuple reading. Instead of holding
/// pre-populated buffers, it holds a reference to the shared RNTupleCategoryState
/// and reads individual collections from disk on demand when
/// getCollectionBuffers() is called. Each lazy read creates (or reuses a cached)
/// partial RNTuple reader with a minimal model containing only the needed fields.
class RNTupleLazyFrameData {
  using CollIDPtr = std::shared_ptr<const podio::CollectionIDTable>;

public:
  RNTupleLazyFrameData() = delete;
  ~RNTupleLazyFrameData() = default;
  RNTupleLazyFrameData(RNTupleLazyFrameData&&) = default;
  RNTupleLazyFrameData& operator=(RNTupleLazyFrameData&&) = default;
  RNTupleLazyFrameData(const RNTupleLazyFrameData&) = delete;
  RNTupleLazyFrameData& operator=(const RNTupleLazyFrameData&) = delete;

  /// Construct from shared category state, entry number, map of available
  /// collections (name -> index in RNTupleCategoryState::collectionInfo),
  /// the shared ID table, and the eagerly-read parameters.
  RNTupleLazyFrameData(std::shared_ptr<RNTupleCategoryState> state, unsigned entry,
                       std::unordered_map<std::string, size_t>&& availableCollections, CollIDPtr idTable,
                       podio::GenericParameters&& params);

  /// Lazily read a single collection from RNTuple. Creates or reuses a cached
  /// partial reader with only this collection's fields and calls LoadEntry().
  std::optional<podio::CollectionReadBuffers> getCollectionBuffers(const std::string& name);

  podio::CollectionIDTable getIDTable() const;

  std::unique_ptr<podio::GenericParameters> getParameters();

  std::vector<std::string> getAvailableCollections() const;

private:
  std::shared_ptr<RNTupleCategoryState> m_state;
  unsigned m_entry{0};
  /// Maps collection name to its index in RNTupleCategoryState::collectionInfo.
  /// Collections are removed from this map once they have been read.
  std::unordered_map<std::string, size_t> m_availableCollections{};
  CollIDPtr m_idTable{nullptr};
  podio::GenericParameters m_parameters{};
};

} // namespace podio

#endif // PODIO_RNTUPLELAZY_FRAMEDATA_H
