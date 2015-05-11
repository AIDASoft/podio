#include "podio/PythonEventStore.h"

PythonEventStore::PythonEventStore(const char* name) :
  m_reader(),
  m_store() {
  m_reader.openFile(name);
  m_store.setReader(&m_reader);
}

podio::CollectionBase* PythonEventStore::get(const char* name) {
  const podio::CollectionBase* coll(nullptr);
  m_store.get(name, coll);
  return const_cast<podio::CollectionBase*>(coll);
}

void PythonEventStore::endOfEvent() {
  m_store.clear();
  m_reader.endOfEvent();
}

void PythonEventStore::goToEvent(unsigned ievent) {
  m_store.clear();
  m_reader.goToEvent(ievent);
}

unsigned PythonEventStore::getEntries() const {
  return m_reader.getEntries();
}
