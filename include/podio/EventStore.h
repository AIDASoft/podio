#ifndef ALBERS_EVENTSTORE_H
#define ALBERS_EVENTSTORE_H

#include <stdexcept>
#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <memory>

// podio specific includes
#include "podio/CollectionIDTable.h"
#include "podio/ICollectionProvider.h"
#include "podio/IMetaDataProvider.h"
#include "podio/GenericParameters.h"


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

  typedef std::map<int,GenericParameters>    RunMDMap;
  typedef std::map<int,GenericParameters>    ColMDMap;

  class EventStore : public ICollectionProvider, public IMetaDataProvider {
    /// Make non-copyable
    EventStore( const EventStore & ) = delete;
    EventStore& operator=( const EventStore & ) = delete;
  public:
    /// Collection entry. Each collection is identified by a name
    typedef std::pair<std::string, CollectionBase*> CollPair;
    typedef std::vector<CollPair> CollContainer;

    EventStore();
    ~EventStore();

    /// create a new collection
    template<typename T>
    T& create(const std::string& name);

    /// register an existing collection
    void registerCollection(const std::string& name, podio::CollectionBase* coll);

    /// access a collection by name. returns true if successful
    template<typename T>
    bool get(const std::string& name, const T*& collection);

    /// fast access to cached collections
    CollectionBase* getFast(unsigned id) const{
      return ( m_cachedCollections.size() > id ? m_cachedCollections[id] : nullptr ) ;
    }

    /// access a collection by ID. returns true if successful
    bool get(unsigned id, CollectionBase*& coll) const override final;

    /// access a collection by name
    /// returns a collection w/ setting isValid to true if successful
    template<typename T>
    const T& get(const std::string& name );

    /// empties collections.
    void clearCollections();

    /// clears itself; deletes collections (use at end of event processing)
    void clear();

    /// Clears only the cache containers (use at end of event if ownership of read objects is transferred)
    void clearCaches();

    /// set the reader
    void setReader(IReader* reader);

    CollectionIDTable* getCollectionIDTable() const { return m_table.get(); }

    virtual bool isValid() const final;

    /// return the event meta data for the current event
    virtual GenericParameters& getEventMetaData() const override ;

    /// return the run meta data for the given runID
    virtual GenericParameters& getRunMetaData(int runID) const override ;

    /// return the collection meta data for the given colID
    virtual GenericParameters& getCollectionMetaData(int colID) const override ;

    RunMDMap* getRunMetaDataMap() const {return &m_runMDMap ; }
    ColMDMap* getColMetaDataMap() const {return &m_colMDMap ; }
    GenericParameters* eventMetaDataPtr() const {return &m_evtMD; }

   private:

    /// get the collection of given name; returns true if successfull
    bool doGet(const std::string& name, CollectionBase*& collection, bool setReferences = true) const;
    /// check if a collection of given name already exists
    bool collectionRegistered(const std::string& name) const;
    void setCollectionIDTable(CollectionIDTable* table) { m_table.reset(table); }

    // members
    mutable std::set<int> m_retrievedIDs;
    mutable CollContainer m_collections;
    mutable std::vector<CollectionBase*> m_cachedCollections;
    IReader* m_reader=nullptr;
    std::unique_ptr<CollectionIDTable> m_table;

    mutable GenericParameters  m_evtMD ;
    mutable RunMDMap m_runMDMap ;
    mutable ColMDMap m_colMDMap ;
  };


template<typename T>
T& EventStore::create(const std::string& name) {
  static_assert(std::is_base_of<podio::CollectionBase,T>::value,
    "DataStore only accepts types inheriting from CollectionBase");
  // TODO: add check for existence
  T* coll = new T();
  registerCollection(name, coll);
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


template<typename T>
const T& EventStore::get(const std::string& name) {
  const T* tmp(0);
  auto success = this->get(name,tmp);
  if (!success) {
    throw std::runtime_error("No collection \'" + name + "\' is present in the EventStore");
  }
  return *tmp;
}

} //namespace
#endif
