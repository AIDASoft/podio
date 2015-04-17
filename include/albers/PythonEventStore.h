#ifndef ALBERS_PYTHONEVENTSTORE_H
#define ALBERS_PYTHONEVENTSTORE_H


#include "albers/EventStore.h"
#include "albers/ROOTReader.h"

class PythonEventStore {
public:
  /// constructor from filename
  PythonEventStore(const char* filename);

  /// access a collection.
  albers::CollectionBase* get(const char* name);

  /// signify end of event
  void endOfEvent();

  /// go to a given event
  void goToEvent(unsigned ievent);

  /// get number of entries in the tree
  unsigned getEntries() const;

 private:
  albers::ROOTReader m_reader;
  albers::EventStore m_store;
};

#endif
