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

/// Struct bundling the buffers that represent a collection for writing.
///
/// data is used by ROOT and holds the address of a unique_ptr.
/// vecPtr holds the raw vector pointer, used by SIO and RNTuple.
struct CollectionWriteBuffers {
  void* data{nullptr};
  void* vecPtr{nullptr};
  CollRefCollection* references{nullptr};
  VectorMembersInfo* vectorMembers{nullptr};

  template <typename T>
  std::vector<T>* dataAsVector() {
    return static_cast<std::vector<T>*>(vecPtr);
  }
};

struct CollectionReadBuffers {
  void* data{nullptr};
  CollRefCollection* references{nullptr};
  VectorMembersInfo* vectorMembers{nullptr};
  SchemaVersionT schemaVersion{0};
  std::string_view type{};

  using CreateFuncT = std::function<std::unique_ptr<podio::CollectionBase>(podio::CollectionReadBuffers&&, bool)>;

  using DeleteFuncT = std::function<void(CollectionReadBuffers&)>;

  CollectionReadBuffers(void* d, CollRefCollection* ref, VectorMembersInfo* vec, SchemaVersionT version,
                        std::string_view typ, CreateFuncT&& createFunc, DeleteFuncT&& deleteFunc) :
      data(d),
      references(ref),
      vectorMembers(vec),
      schemaVersion(version),
      type(typ),
      createCollection(std::move(createFunc)),
      deleteBuffers(std::move(deleteFunc)) {
  }

  CollectionReadBuffers() = default;

  // Copying is not allowed since we own the buffers
  CollectionReadBuffers(const CollectionReadBuffers&) = delete;
  CollectionReadBuffers& operator=(const CollectionReadBuffers&) = delete;

  CollectionReadBuffers(CollectionReadBuffers&& other) :
      data(other.data),
      references(other.references),
      vectorMembers(other.vectorMembers),
      schemaVersion(other.schemaVersion),
      type(other.type),
      createCollection(std::move(other.createCollection)),
      deleteBuffers(std::move(other.deleteBuffers)) {
    other.data = nullptr;
    other.references = nullptr;
    other.vectorMembers = nullptr;
    other.schemaVersion = 0;
    other.type = {};
  }

  CollectionReadBuffers& operator=(CollectionReadBuffers&& other) {
    if (this != &other) {
      // Clean up our own buffers first
      if (deleteBuffers) {
        deleteBuffers(*this);
      }
      data = other.data;
      references = other.references;
      vectorMembers = other.vectorMembers;
      schemaVersion = other.schemaVersion;
      type = other.type;
      createCollection = std::move(other.createCollection);
      deleteBuffers = std::move(other.deleteBuffers);

      other.data = nullptr;
      other.references = nullptr;
      other.vectorMembers = nullptr;
      other.schemaVersion = 0;
      other.type = {};
    }
    return *this;
  }

  ~CollectionReadBuffers() {
    if (deleteBuffers) {
      deleteBuffers(*this);
    }
  }

  CollectionReadBuffers(CollectionWriteBuffers buffers) :
      data(buffers.data), references(buffers.references), vectorMembers(buffers.vectorMembers) {
  }

  template <typename DataT>
  std::vector<DataT>* dataAsVector() {
    return asVector<DataT>(data);
  }

  template <typename T>
  static std::vector<T>* asVector(void* raw) {
    return static_cast<std::vector<T>*>(raw);
  }

  CreateFuncT createCollection{};

  // Type-erased deleter for the owned buffers. Set at generation time since
  // only the generated code knows the concrete types behind the void pointers
  DeleteFuncT deleteBuffers{};
};

} // namespace podio

#endif // PODIO_COLLECTIONBUFFERS_H
