#ifndef PODIO_IREADER_H
#define PODIO_IREADER_H

#include "podio/podioVersion.h"

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <vector>

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
  virtual ~IReader() = default;
  /// Read Collection of given name
  /// Does not set references yet.
  virtual CollectionBase* readCollection(const std::string& name) = 0;
  /// Get CollectionIDTable of read-in data
  virtual std::shared_ptr<CollectionIDTable> getCollectionIDTable() = 0;
  /// read event meta data from file
  virtual GenericParameters* readEventMetaData() = 0;
  virtual std::map<int, GenericParameters>* readCollectionMetaData() = 0;
  virtual std::map<int, GenericParameters>* readRunMetaData() = 0;
  /// get the number of events available from this reader
  virtual unsigned getEntries() const = 0;
  /// Prepare the reader to read the next event
  virtual void endOfEvent() = 0;
  // TODO: decide on smart-pointers for passing of objects
  /// Check if reader is valid
  virtual bool isValid() const = 0;

  virtual void openFile(const std::string& filename) = 0;
  virtual void closeFile() = 0;

  virtual void readEvent() = 0;
  virtual void goToEvent(unsigned iEvent) = 0;

  /// Get the podio version with which the current file has been written
  virtual podio::version::Version currentFileVersion() const = 0;
};

} // namespace podio

#endif
