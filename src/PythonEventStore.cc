#include "podio/PythonEventStore.h"

#include <fstream>
#include <iostream>
#include <string>

podio::PythonEventStore::PythonEventStore(const char* name) : m_reader(), m_store() {
  std::ifstream inputfile(name);
  m_isZombie = inputfile.good() ? false : true;
  // the file could be a remote file that we cannot access but root
  // knows how to handle via the xrootd protocol.
  // if that is the case we ignore m_isZombie.
  if (!std::string("root:/").compare(0, 6, name, 6)) {
    m_isZombie = false;
  }
  if (!m_isZombie) {
    m_reader.openFiles({std::string(name)});
    m_store.setReader(&m_reader);
  }
}

const podio::CollectionBase* podio::PythonEventStore::get(const char* name) {
  const podio::CollectionBase* coll(nullptr);
  m_store.get(name, coll);
  return coll;
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
