#ifndef ALBERS_PYTHONEVENTSTORE_H
#define ALBERS_PYTHONEVENTSTORE_H


#include "podio/EventStore.h"
#include "podio/ROOTReader.h"

namespace podio {

class PythonEventStore {
public:
  /// constructor from filename
  PythonEventStore(const char* filename);

  /// access a collection.
  podio::CollectionBase* get(const char* name);

  /// signify end of event
  void endOfEvent();

  /// go to a given event
  void goToEvent(unsigned ievent);

  /// get number of entries in the tree
  unsigned getEntries() const;

  /// is the input file accessible?
  bool isZombie() const {return m_isZombie;}
  
  bool isValid() const {return m_reader.isValid();}
  void close() {m_reader.closeFiles();}

  /// list available collections
  const std::vector<std::string>& getCollectionNames() const;

 private:
  podio::ROOTReader m_reader;
  podio::EventStore m_store;

  /// set to true if input root file accessible, false otherwise
  bool              m_isZombie;
};

}

#endif
