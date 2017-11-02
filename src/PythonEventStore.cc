#include "podio/PythonEventStore.h"

#include <fstream>

podio::PythonEventStore::PythonEventStore(const char* name) :
  m_reader(),
  m_store() {
  std::ifstream inputfile(name);
  m_isZombie = inputfile.good() ? false : true;
  if(! m_isZombie) {
    m_reader.openFile(name);
    m_store.setReader(&m_reader);
  }
}

podio::CollectionBase* podio::PythonEventStore::get(const char* name) {
  const podio::CollectionBase* coll(nullptr);
  auto success = m_store.get(name, coll);
  return const_cast<podio::CollectionBase*>(coll);
}

void podio::PythonEventStore::endOfEvent() {
  m_store.clear();
  m_reader.endOfEvent();
}

void podio::PythonEventStore::goToEvent(unsigned ievent) {
  m_store.clear();
  m_reader.goToEvent(ievent);
}

unsigned podio::PythonEventStore::getEntries() const {
  return m_reader.getEntries();
}

const std::vector<std::string>& podio::PythonEventStore::getCollectionNames() const {
  return m_store.getCollectionIDTable()->names();
}
