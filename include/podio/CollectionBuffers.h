#ifndef PODIO_COLLECTIONBUFFERS_H
#define PODIO_COLLECTIONBUFFERS_H

#include "podio/ObjectID.h"

#include <string>
#include <utility>
#include <vector>
#include <memory>


namespace podio {

template<typename T>
using UVecPtr = std::unique_ptr<std::vector<T>>;

using CollRefCollection = std::vector<UVecPtr<podio::ObjectID>>;
using VectorMembersInfo = std::vector<std::pair<std::string, void*>>;

/**
 * Simple helper struct that bundles all the potentially necessary buffers that
 * are necessary to represent a collection for I/O purposes.
 */
struct CollectionBuffers {
  void* data{nullptr};
  CollRefCollection* references{nullptr};
  VectorMembersInfo* vectorMembers{nullptr};

  template<typename DataT>
  std::vector<DataT>* dataAsVector() {
    // Are we at a beach? I can almost smell the C...
    return *static_cast<std::vector<DataT>**>(data);
  }
};

}

#endif // PODIO_COLLECTIONBUFFERS_H
