#ifndef REGISTRY_H
#define REGISTRY_H

#include <algorithm>
#include <functional>
#include <string>
#include <vector>
#include <mutex>
#include <iostream>

// albers specific includes

/*
The Registry knows about the position of
transient collections and all the buffers
in memory. If some collection/buffer is missing,
it may ask the ICollectionProvider for help depending
on whether the lookup is 'lazy' or not.

*/

namespace albers {

  class ICollectionProvider;
  class CollectionBase;

  class Registry{

  public:

  Registry():
    m_addresses(), m_collectionIDs(), m_names()
  {};

    // get the Data address but don't do anything else
    template<typename T>
    void lazyGetDataAddressFromID(int ID, T*&) const;

    template<typename T>
    void getCollectionFromID(int ID, T*&) const;

    template<typename T>
    void getCollectionFromName(const std::string& name, T*&) const;

    template<typename T>
    void setDataAddress(int ID, T* address);

    template<typename T>
    void setDataAddress(const std::string& name, T* address);

    template<typename Coll, typename Data>
    void setCollectionAddresses(const std::string& name, Coll* collAddress, Data* dataAddress);

    std::string getNameFromID(int ID) const {
      //std::lock_guard<std::recursive_mutex> lock(m_mutex);
      auto result = std::find(begin(m_collectionIDs), end(m_collectionIDs), ID);
      auto index = result - m_collectionIDs.begin();
      return m_names[index];
    };

    template<typename T>
    unsigned registerData(T* address, const std::string& name); // returns the ID

    void resetAddresses(){std::fill(m_addresses.begin(), m_addresses.end(), nullptr);};

    void setCollectionProvider(ICollectionProvider* provider) {m_collectionProvider = provider;};
    ICollectionProvider* collectionProvider(){return m_collectionProvider;};

    std::vector<std::string>& names(){ return m_names;};

    /// Prints collection information
    void print() const;

  private:
    void doGetDataAddressFromID(int ID, void*& address) const;
    std::vector<void*>            m_addresses;
    std::vector<CollectionBase*>  m_collections;
    std::vector<int>              m_collectionIDs;
    std::vector<std::string>      m_names;
    ICollectionProvider*          m_collectionProvider;
    //mutable std::recursive_mutex   m_mutex;
  };

template<typename T>
void Registry::lazyGetDataAddressFromID(int ID, T*& address) const {
  //std::lock_guard<std::recursive_mutex> lock(m_mutex);
  auto result = std::find(begin(m_collectionIDs), end(m_collectionIDs), ID);
  if (result == end(m_collectionIDs)){
    address = nullptr;
  } else {
    auto index = result - m_collectionIDs.begin();
    address = static_cast<T*>(m_addresses[index]);
  }
}

template<typename T>
void Registry::getCollectionFromName(const std::string& name, T*& collection) const {
  //std::lock_guard<std::recursive_mutex> lock(m_mutex);
  auto result = std::find(begin(m_names), end(m_names), name);
  if (result == end(m_names)){
    collection = nullptr;
  } else {
    auto index = result - m_names.begin();
    collection = static_cast<T*>(m_collections[index]);
  }
}

template<typename T>
void Registry::getCollectionFromID(int ID, T*& collection) const {
  //std::lock_guard<std::recursive_mutex> lock(m_mutex);
  void* tmp;
  doGetDataAddressFromID(ID, tmp);
  auto result = std::find(begin(m_collectionIDs), end(m_collectionIDs), ID);
  auto index = result - m_collectionIDs.begin();
  collection = static_cast<T*>(m_collections[index]); //TODO: buggy!!!
}

template<typename T>
void Registry::setDataAddress(int ID, T* address){
  //std::lock_guard<std::recursive_mutex> lock(m_mutex);
  auto result = std::find(begin(m_collectionIDs), end(m_collectionIDs), ID);
  auto index = result - m_collectionIDs.begin();
  auto bare_address = static_cast<void*>(address);
  m_addresses[index] = bare_address;
}

template<typename T>
void Registry::setDataAddress(const std::string& name, T* address){
  //std::lock_guard<std::recursive_mutex> lock(m_mutex);
  auto result = std::find(begin(m_names), end(m_names), name);
  auto index = result - m_names.begin();
  auto bare_address = static_cast<void*>(address);
  m_addresses[index] = bare_address;
}

template<typename Coll, typename Data>
void Registry::setCollectionAddresses(const std::string& name, Coll* collAddress, Data* dataAddress) {
  //std::lock_guard<std::recursive_mutex> lock(m_mutex);
  auto result = std::find(begin(m_names), end(m_names), name);
  auto index = result - m_names.begin();
  auto bare_address = static_cast<void*>(dataAddress);
  m_addresses[index] = bare_address;
  if (int(m_collections.size()) < index+1) {
    m_collections.resize(index+1);
  }
  m_collections[index] = static_cast<CollectionBase*>(collAddress);
}

template<typename T>
unsigned Registry::registerData(T* collection, const std::string& name){
  //std::lock_guard<std::recursive_mutex> lock(m_mutex);
  auto bare_address = static_cast<void*>(collection->_getBuffer());
  auto result = std::find(begin(m_addresses), end(m_addresses), bare_address);
  int ID = 0;
  if (result == m_addresses.end()) {
      //TODO: find a proper hashing function fitting in an int.
      m_addresses.emplace_back(bare_address);
      m_names.emplace_back(name);
      m_collections.emplace_back(collection);
      ID = m_names.size();
      m_collectionIDs.emplace_back( ID );
      collection->setID(ID);
   } else {
    auto index = result - m_addresses.begin();
    ID = m_collectionIDs[index];
   }
  return ID;
}

} //namespace
#endif
