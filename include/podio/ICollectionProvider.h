#ifndef ICOLLECTIONPROVIDER_H
#define ICOLLECTIONPROVIDER_H

namespace podio {

  class CollectionBase;

  class ICollectionProvider {
  public:
    /// destructor
    virtual ~ICollectionProvider(){};
    /// access a collection by ID. returns true if successful
    virtual bool get(unsigned collectionID, CollectionBase*& collection) const = 0;
  };

} // namespace

#endif
