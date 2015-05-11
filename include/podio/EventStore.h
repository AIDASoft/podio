#ifndef ALBERS_EVENTSTORE_H
#define ALBERS_EVENTSTORE_H

#include <string>
#include <vector>
#include <type_traits>
#include <set>

// podio specific includes
#include "podio/CollectionIDTable.h"
#include "podio/ICollectionProvider.h"

/**
This is an *example* event store

The event store holds the object collections.

It is used to create new collections, and to access existing ones.
When accessing a collection that is not yet in the event store,
the event store makes use of a Reader to read the collection.

**/

namespace podio {

  class CollectionBase;
  class IReader;

  class EventStore : public ICollectionProvider {

  public:
    /// Collection entry. Each collection is identified by a name
    typedef std::pair<std::string, CollectionBase*> CollPair;
    typedef std::vector<CollPair> CollContainer;

    EventStore();
    ~EventStore();

    /// create a new collection
    template<typename T>
    T& create(const std::string& name);

    /// access a collection by name. returns true if successful
    template<typename T>
    bool get(const std::string& name, const T*& collection);

    /// access a collection by ID. returns true if successful
    bool get(int id, CollectionBase*& coll) const;

    /// empties collections.
    void clearCollections();

    /// clears itself; deletes collections
    void clear();

    /// set the reader
    void setReader(IReader* reader);

    CollectionIDTable* getCollectionIDTable(){return m_table;};

  private:

    /// get the collection of given name; returns true if successfull
    bool doGet(const std::string& name, CollectionBase*& collection, bool setReferences = true) const;
    /// check if a collection of given name already exists
    bool collectionRegistered(const std::string& name) const;
    void setCollectionIDTable(CollectionIDTable* table){if (m_table!=nullptr) delete m_table; m_table=table;};

    // members
    mutable std::set<int> m_retrievedIDs;
    mutable CollContainer m_collections;
    IReader* m_reader;
    CollectionIDTable* m_table;
  };


template<typename T>
T& EventStore::create(const std::string& name) {
  static_assert(std::is_base_of<podio::CollectionBase,T>::value,
    "DataStore only accepts types inheriting from CollectionBase");
  // TODO: add check for existence
  T* coll = new T();
  m_collections.push_back({name,coll});
  auto id = m_table->add(name);
  coll->setID(id);
  return *coll;
}

template<typename T>
bool EventStore::get(const std::string& name, const T*& collection){
  //  static_assert(std::is_base_of<podio::CollectionBase,T>::value,
  //              "DataStore only contains types inheriting from CollectionBase");
  CollectionBase* tmp(0);
  doGet(name, tmp);
  collection = static_cast<T*>(tmp);
  if (collection != nullptr) { return true;}
  return false;
}

} //namespace
#endif
