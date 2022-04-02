#ifndef PODIO_ICOLLECTIONPROVIDER_H
#define PODIO_ICOLLECTIONPROVIDER_H

namespace podio {

class CollectionBase;

class ICollectionProvider {
public:
  /// destructor
  virtual ~ICollectionProvider() = default;
  /// access a collection by ID. returns true if successful
  virtual bool get(int collectionID, CollectionBase*& collection) const = 0;
};

} // namespace podio

#endif
