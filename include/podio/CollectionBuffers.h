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
                        std::string_view typ, CreateFuncT&& createFunc, RecastFuncT&& recastFunc,
                        DeleteFuncT&& deleteFunc) :
      data(d),
      references(ref),
      vectorMembers(vec),
      schemaVersion(version),
      type(typ),
      createCollection(std::move(createFunc)),
      recast(std::move(recastFunc)),
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

  // This is a hacky workaround for the ROOT backend at the moment. There is
  // probably a better solution, but I haven't found it yet. The problem is the
  // following:
  //
  // When creating a pointer to a vector<T>, either via new or via
  // TClass::New(), we get a void*, that can be cast back to a vector with
  //
  //     static_cast<vector<T>*>(raw);
  //
  // However, as soon as we pass that same void* to TBranch::SetAddress this no
  // longer works and the actual cast has to be
  //
  //     *static_cast<vector<T>**>(raw);
  //
  // To make it possible to always use the first form, after we leave the Root
  // parts of reading, this function is populated in the createBuffers call of each
  // datatype where we have the necessary type information (from code
  // generation) to do the second cast and assign the result of that to the data
  // field again.
  RecastFuncT recast{};

  // Workaround for https://github.com/AIDASoft/podio#500
  // We need a function that explicitly deletes the buffers, but for this we
  // need type information, so we attach a delete function at generation time
  DeleteFuncT deleteBuffers{};
};

} // namespace podio

#endif // PODIO_COLLECTIONBUFFERS_H
