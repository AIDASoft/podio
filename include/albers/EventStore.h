#ifndef ALBERS_EVENTSTORE_H
#define ALBERS_EVENTSTORE_H

#include <string>
#include <vector>
#include <type_traits>

// albers specific includes
#include "albers/Registry.h"

/**
This is an *example* event store

The event store holds the object collections.

It is used to create new collections, and to access existing ones.
When accessing a collection that is not yet in the event store,
the event store makes use of a Reader to read the collection.

**/

namespace albers {

  class CollectionBase;
  class ROOTReader;

  class EventStore {

  public:
    /// Collection entry. Each collection is identified by a name
    typedef std::pair<std::string, CollectionBase*> CollPair;
    typedef std::vector<CollPair> CollContainer;

    EventStore(Registry* registry);
    ~EventStore();

    /// create a new collection
    template<typename T>
      T& create(const std::string& name);

    /// access a collection. returns true if successfull
    template<typename T>
      bool get(const std::string& name, const T*& collection);

    /// clears all collections. COLIN: choose a different name?
    void next();

    void endOfEvent();

    /// set the reader
    void setReader(ROOTReader* reader);

    bool doGet(const std::string& name, CollectionBase*& collection) const;

  private:
    // members
    mutable CollContainer m_collections;
    ROOTReader* m_reader;
    Registry* m_registry;
  };


template<typename T>
T& EventStore::create(const std::string& name) {
  static_assert(std::is_base_of<CollectionBase,T>::value,
    "DataStore only accepts types inheriting from CollectionBase");
  // TODO: add check for existence
  T* coll = new T();
  m_collections.push_back({name,coll});
  auto id = m_registry->registerData(coll, name);
  coll->setID(id);
  return *coll;
}

template<typename T>
bool EventStore::get(const std::string& name, const T*& collection){
  static_assert(std::is_base_of<CollectionBase,T>::value,
                "DataStore only contains types inheriting from CollectionBase");
  CollectionBase* tmp(0);
  doGet(name, tmp);
  collection = static_cast<T*>(tmp);
  if (collection != nullptr) { return true;}
  return false;
}

} //namespace
#endif
