#ifndef ICOLLECTIONPROVIDER_H
#define ICOLLECTIONPROVIDER_H

namespace albers {

  class CollectionBase;

  class ICollectionProvider {
  public:
    /// destructor
    virtual ~ICollectionProvider(){};
    /// access a collection by ID. returns true if successful
    virtual bool get(int collectionID, CollectionBase*& collection) const = 0;
  };

} // namespace

#endif
