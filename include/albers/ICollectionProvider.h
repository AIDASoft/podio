#ifndef ICOLLECTIONPROVIDER_H
#define ICOLLECTIONPROVIDER_H

namespace albers {

class CollectionBase;

class ICollectionProvider {
  public:
    virtual ~ICollectionProvider(){};
    virtual bool get(int collectionID, CollectionBase*& collection) const = 0;
};

} // namespace

#endif
