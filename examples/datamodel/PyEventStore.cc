#include "PyEventStore.h"

#include "albers/EventStore.h"
#include "albers/Reader.h"
#include "albers/CollectionBase.h"

PyEventStore::PyEventStore(const char* name) {
  m_reader = new albers::Reader();
  m_store = new albers::EventStore(nullptr);
  m_store->setReader(m_reader);
  m_reader->openFile(name);
}

albers::CollectionBase* PyEventStore::get(const char* name) {
  albers::CollectionBase* coll(nullptr);
  bool available = m_store->doGet(name, coll);
  return coll;
}

void PyEventStore::endOfEvent() {
  m_store->endOfEvent();
  m_reader->endOfEvent();
}

void PyEventStore::goToEvent(unsigned ievent) {
  m_store->endOfEvent();
  m_reader->goToEvent(ievent);
}

unsigned PyEventStore::getEntries() const { 
  return m_reader->getEntries();
}



