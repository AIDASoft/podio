#include "albers/PythonEventStore.h"

PythonEventStore::PythonEventStore(const char* name) :
  m_reader(),
  m_store() {
  m_reader.openFile(name);
  m_store.setReader(&m_reader);
}

albers::CollectionBase* PythonEventStore::get(const char* name) {
  const albers::CollectionBase* coll(nullptr);
  m_store.get(name, coll);
  return const_cast<albers::CollectionBase*>(coll);
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
