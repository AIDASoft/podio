#ifndef ICOLLECTIONPROVIDER_H
#define ICOLLECTIONPROVIDER_H

#include <algorithm>
#include <string>
#include <vector>

#include <iostream>
/*

This class has the function to read available data from disk
and to prepare collections and buffers.
Once data are there it lets the Registry know.

 */

namespace albers {

class CollectionBase;

class ICollectionProvider {
  public:
    virtual ~ICollectionProvider(){};
//    virtual bool getCollection(const std::string& name,
//		       const CollectionBase*& collection) = 0;
    virtual void* getBuffer(const unsigned collectionID) = 0;
};

} // namespace

#endif
