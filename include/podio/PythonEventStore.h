#ifndef PODIO_PYTHONEVENTSTORE_H
#define PODIO_PYTHONEVENTSTORE_H

#include "podio/EventStore.h"
#include "podio/GenericParameters.h"
#include "podio/IReader.h"

#include <memory>

namespace podio {

class PythonEventStore {
public:
  /// constructor from filename
  PythonEventStore(const char* filename);

  /// access a collection.
  const podio::CollectionBase* get(const char* name);

  /// signify end of event
  void endOfEvent();

  /// go to a given event
  void goToEvent(unsigned ievent);

  /// get number of entries in the tree
  unsigned getEntries() const;

  /// is the input file accessible?
  bool isZombie() const {
    return m_isZombie;
  }

  bool isValid() const {
    return m_reader && m_reader->isValid();
  }
  void close() {
    m_reader->closeFile();
  }

  /// list available collections
  const std::vector<std::string>& getCollectionNames() const;

  const podio::GenericParameters& getEventMetaData() {
    return m_store.getEventMetaData();
  }

private:
  std::unique_ptr<podio::IReader> m_reader;
  podio::EventStore m_store;

  /// set to true if input root file accessible, false otherwise
  bool m_isZombie{true};
};

} // namespace podio

#endif
