#ifndef PODIO_COLLECTIONBUFFERFACTORY_H
#define PODIO_COLLECTIONBUFFERFACTORY_H

#include "podio/CollectionBuffers.h"
#include "podio/SchemaEvolution.h"

#include <functional>
#include <optional>
#include <unordered_map>
#include <vector>

namespace podio {

/**
 * Registry like type that provides empty buffers for collections
 */
class CollectionBufferFactory {
  /// Internal storage is a map to an array of creation functions, where the
  /// version determines the place in that array. This should be a viable
  /// approach because we now the "latest and greatest" schema version
  using CreationFuncT = std::function<podio::CollectionReadBuffers(bool)>;
  using VersionMapT = std::vector<CreationFuncT>;
  using MapT = std::unordered_map<std::string, VersionMapT>;

public:
  CollectionBufferFactory(CollectionBufferFactory const&) = delete;
  CollectionBufferFactory& operator=(CollectionBufferFactory const&) = delete;
  CollectionBufferFactory(CollectionBufferFactory&&) = delete;
  CollectionBufferFactory& operator=(CollectionBufferFactory&&) = delete;
  ~CollectionBufferFactory() = default;

  static CollectionBufferFactory& mutInstance();
  static CollectionBufferFactory const& instance();

  std::optional<podio::CollectionReadBuffers> createBuffers(const std::string& collType, SchemaVersionT version,
                                                            bool subsetColl) const;

  void registerCreationFunc(const std::string& collType, SchemaVersionT version, const CreationFuncT& creationFunc);

private:
  CollectionBufferFactory() = default;

  MapT m_funcMap{};
};

} // namespace podio

#endif // PODIO_COLLECTIONBUFFERFACTORY_H
