#ifndef PODIO_ICOLLECTIONPROVIDER_H
#define PODIO_ICOLLECTIONPROVIDER_H

#include <cstdint>

namespace podio {

class CollectionBase;

class ICollectionProvider {
public:
  /// destructor
  virtual ~ICollectionProvider() = default;
  /// access a collection by ID. returns true if successful
  virtual bool get(uint32_t collectionID, CollectionBase*& collection) const = 0;
};

} // namespace podio

#endif
