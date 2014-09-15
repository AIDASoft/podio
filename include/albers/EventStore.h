#ifndef ALBERS_EVENTSTORE_H
#define ALBERS_EVENTSTORE_H

#include <string>
#include <vector>
// albers specific includes
#include "albers/Registry.h"

namespace albers {

  class CollectionBase;
  class Reader;
  class Writer; 

  class EventStore {

  public:
    typedef std::pair<std::string, CollectionBase*> CollPair;

    EventStore(Registry* registry);
    ~EventStore();

    template<typename T>
      T& create(const std::string& name);

    template<typename T>
      bool get(const std::string& name, T*& collection);

    // If one wants to know all what is in there
    // Use case is e.g. a WriteAlg.
    std::vector<CollPair>& get_content();

    void next();
    void setReader(Reader* reader){m_reader = reader;};

  private:
    // methods
    bool doGet(const std::string& name, CollectionBase*& collection) const;
    // members
    mutable std::vector<CollPair> m_collections;
    Reader* m_reader;
    Writer* m_writer;
    Registry* m_registry;
  };


template<typename T>
T& EventStore::create(const std::string& name) {
  // TODO: add check for existence
  T* coll = new T();
  m_collections.emplace_back(std::make_pair(name,coll));    
  m_registry->registerPOD(coll->_getBuffer(),name);
  return *coll;
}

template<typename T>
bool EventStore::get(const std::string& name, T*& collection){
  CollectionBase* tmp;
  doGet(name, tmp);
  collection = dynamic_cast<T*>(tmp);
  if (collection != nullptr) { return true;}
  return false;
}

} //namespace
#endif
