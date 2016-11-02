#ifndef PODIO_PYTHONEVENTSTORE_H
#define PODIO_PYTHONEVENTSTORE_H


#include "podio/EventStore.h"
#include "podio/ROOTReader.h"
#include "podio/ROOTWriter.h"

namespace podio {

class PythonEventStore {
public:
  /// constructor from filename
  PythonEventStore(const char* filename);

  /// access a collection.
  podio::CollectionBase* get(const char* name);

  /// register a collection
  void registerCollection(const char* name, podio::CollectionBase* coll);

  /// signify end of event
  void endOfEvent();

  /// go to a given event
  void goToEvent(unsigned ievent);

  /// get number of entries in the tree
  unsigned getEntries() const;

  bool isValid() const {return m_reader.isValid();}
  void close() {m_reader.closeFile();}

  /// list available collections
  const std::vector<std::string>& getCollectionNames() const;

 private:
  podio::ROOTReader m_reader;
  podio::EventStore m_store;
  //  podio::ROOTWriter m_writer;
};

}

#endif
