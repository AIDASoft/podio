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

namespace podio {

class CollectionBase;
class CollectionIDTable;

class IReader {
  public:
    virtual ~IReader(){};
    /// Read Collection of given name
    virtual CollectionBase* readCollection(const std::string& name) = 0;
    /// Get CollectionIDTable of read-in data
    virtual CollectionIDTable* getCollectionIDTable() = 0;
    //TODO: decide on smart-pointers for passing of objects
};

} // namespace

#endif
