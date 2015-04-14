#ifndef IREADER_H
#define IREADER_H

#include <algorithm>
#include <string>
#include <vector>

#include <iostream>

/*

This class has the function to read available data from disk
and to prepare collections and buffers.

 */

namespace albers {

class CollectionBase;
class CollectionIDTable;

class IReader {
  public:
    virtual ~IReader(){};
    virtual bool getCollection(const std::string& name, CollectionBase*& collection) = 0;
    virtual CollectionIDTable* getCollectionIDTable() {return nullptr;};
    virtual void* getBuffer(const unsigned collectionID) = 0;
};

} // namespace

#endif
