#ifndef PODIO_COLLECTIONBUFFERS_H
#define PODIO_COLLECTIONBUFFERS_H

#include "podio/ObjectID.h"
#include "podio/SchemaEvolution.h"

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace podio {

class CollectionBase;

template <typename T>
using UVecPtr = std::unique_ptr<std::vector<T>>;

using CollRefCollection = std::vector<UVecPtr<podio::ObjectID>>;
using VectorMembersInfo = std::vector<std::pair<std::string, void*>>;

/// Simple helper struct that bundles all the potentially necessary buffers that
/// are necessary to represent a collection for I/O purposes.
struct CollectionWriteBuffers {
  void* data{nullptr};
  void* vecPtr{nullptr};
  CollRefCollection* references{nullptr};
  VectorMembersInfo* vectorMembers{nullptr};

  template <typename DataT>
  std::vector<DataT>* dataAsVector() {
    return asVector<DataT>(data);
  }

  template <typename T>
  static std::vector<T>* asVector(void* raw) {
    // Are we at a beach? I can almost smell the C...
    return *static_cast<std::vector<T>**>(raw);
  }
};

struct CollectionReadBuffers {
  void* data{nullptr};
  CollRefCollection* references{nullptr};
  VectorMembersInfo* vectorMembers{nullptr};
  SchemaVersionT schemaVersion{0};
  std::string_view type{};

  using CreateFuncT = std::function<std::unique_ptr<podio::CollectionBase>(podio::CollectionReadBuffers, bool)>;
  using RecastFuncT = std::function<void(CollectionReadBuffers&)>;

  using DeleteFuncT = std::function<void(CollectionReadBuffers&)>;

  CollectionReadBuffers(void* d, CollRefCollection* ref, VectorMembersInfo* vec, SchemaVersionT version,
                        std::string_view typ, CreateFuncT&& createFunc, RecastFuncT&&,
                        DeleteFuncT&& deleteFunc) :
      data(d),
      references(ref),
      vectorMembers(vec),
      schemaVersion(version),
      type(typ),
      createCollection(std::move(createFunc)),
      deleteBuffers(std::move(deleteFunc)) {
  }

  CollectionReadBuffers() = default;
  CollectionReadBuffers(const CollectionReadBuffers&) = default;
  CollectionReadBuffers& operator=(const CollectionReadBuffers&) = default;

  CollectionReadBuffers(CollectionWriteBuffers buffers) :
      data(buffers.data), references(buffers.references), vectorMembers(buffers.vectorMembers) {
  }

  template <typename DataT>
  std::vector<DataT>* dataAsVector() {
    return asVector<DataT>(data);
  }

  template <typename T>
  static std::vector<T>* asVector(void* raw) {
    // Are we at a beach? I can almost smell the C...
    return static_cast<std::vector<T>*>(raw);
  }

  CreateFuncT createCollection{};

  RecastFuncT recast{};
  // Workaround for https://github.com/AIDASoft/podio/issues/500
  // We need a function that explicitly deletes the buffers, but for this we
  // need type information, so we attach a delete function at generation time
  DeleteFuncT deleteBuffers{};
};

} // namespace podio

#endif // PODIO_COLLECTIONBUFFERS_H
