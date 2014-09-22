#ifndef REGISTRY_H
#define REGISTRY_H

#include <algorithm>
#include <functional>
#include <string>
#include <vector>

// albers specific includes

/*
The Registry knows about the position of 
transient collections and all the buffers 
in memory. If some collection/buffer is missing, 
it may ask the Reader for help depending on whether
the lookup is 'lazy' or not.

COLIN: when getting a collection, it is in fact the EventStore that asks the Reader for help, not the Registry. Are the lines above referring to something else?

COLIN: it seems the registry also keeps track of the collection IDs, which might play a role in the Writer.
*/

#include <iostream>

namespace albers {

  class Reader;

  class Registry{

  public:

  Registry():
    m_addresses(), m_collectionIDs(), m_names(), m_reader()
  {};    

    // get the pod address but don't do anything else
    template<typename T>
    void lazyGetPODAddressFromID(int ID, T*&) const;

    template<typename T>
    void getPODAddressFromID(int ID, T*&) const;

    template<typename T>
    int getIDFromPODAddress(T* address) const;    

    template<typename T>
    void setPODAddress(int ID, T* address);

    template<typename T>
    void setPODAddress(const std::string& name, T* address); 

    std::string getNameFromID(const int ID) const {
      auto result = std::find(begin(m_collectionIDs), end(m_collectionIDs), ID);
      auto index = result - m_collectionIDs.begin();
      return m_names[index];      
    };

    template<typename T>
    int registerPOD(T* address, const std::string& name); // returns the ID

    void resetAddresses(){std::fill(m_addresses.begin(), m_addresses.end(), nullptr);};

    void setReader(Reader* reader) {m_reader = reader;};
    Reader* reader(){return m_reader;};
      
    std::vector<std::string>& names(){ return m_names;};

  private:
    void doGetPODAddressFromID(int ID, void*& address) const;    
    std::vector<void*>  m_addresses; 
    std::vector<int>    m_collectionIDs;
    std::vector<std::string> m_names;
    Reader*                  m_reader; //! transient
  };

template<typename T>
  void Registry::lazyGetPODAddressFromID(int ID, T*& address) const {
  auto result = std::find(begin(m_collectionIDs), end(m_collectionIDs), ID);  
  if (result == end(m_collectionIDs)){
    address = nullptr;
  } else {
    auto index = result - m_collectionIDs.begin();
    address = static_cast<T*>(m_addresses[index]);
  }
} 

template<typename T>
void Registry::getPODAddressFromID(int ID, T*& address) const {
  void* tmp;
  doGetPODAddressFromID(ID, tmp);
  address = static_cast<T*>(tmp);
 }

template<typename T>
int Registry::getIDFromPODAddress(T* address) const {
  auto bare_address = static_cast<void*>(address);
  auto result = std::find(begin(m_addresses), end(m_addresses), bare_address);
  auto index = result - m_addresses.begin();  
  return m_collectionIDs[index];
}

template<typename T>
void Registry::setPODAddress(int ID, T* address){
  auto result = std::find(begin(m_collectionIDs), end(m_collectionIDs), ID);
  auto index = result - m_collectionIDs.begin();
  auto bare_address = static_cast<void*>(address);
  m_addresses[index] = bare_address;;
}

template<typename T>
void Registry::setPODAddress(const std::string& name, T* address){
  auto result = std::find(begin(m_names), end(m_names), name);
  auto index = result - m_names.begin();
  auto bare_address = static_cast<void*>(address);
  m_addresses[index] = bare_address;;
}

template<typename T>
int Registry::registerPOD(T* address, const std::string& name){
  auto bare_address = static_cast<void*>(address);
  auto result = std::find(begin(m_addresses), end(m_addresses), bare_address);
  int ID = 0;
  if (result == m_addresses.end()) {
      std::hash<std::string> hash;
      m_addresses.emplace_back(bare_address);
      m_names.emplace_back(name);    
      ID = hash(name); 
      m_collectionIDs.emplace_back( ID );
   } else {
    auto index = result - m_addresses.begin();
    ID = m_collectionIDs[index];
   }
  return ID;
}

} //namespace
#endif
