#ifndef IREADER_H
#define IREADER_H

#include <algorithm>
#include <string>
#include <vector>
#include <map>

#include <iostream>

/*

This class has the function to read available data from disk
and to prepare collections and buffers.

*/

namespace podio {

class CollectionBase;
class CollectionIDTable;
class GenericParameters;

class IReader {
  public:
    virtual ~IReader(){};
    /// Read Collection of given name
    /// Does not set references yet.
    virtual CollectionBase* readCollection(const std::string& name) = 0;
    /// Get CollectionIDTable of read-in data
    virtual CollectionIDTable* getCollectionIDTable() = 0;
    /// read event meta data from file
    virtual GenericParameters* readEventMetaData()=0 ;
    virtual std::map<int,GenericParameters>* readCollectionMetaData()=0 ;
    virtual std::map<int,GenericParameters>* readRunMetaData()=0 ;
    /// get the number of events available from this reader
    virtual unsigned getEntries() const = 0;
    /// Prepare the reader to read the next event
    virtual void endOfEvent() = 0;
    //TODO: decide on smart-pointers for passing of objects
    /// Check if reader is valid
    virtual bool isValid() const = 0;
};

} // namespace

#endif
