//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef ExampleMCCollection_H
#define  ExampleMCCollection_H

#include <string>
#include <vector>
#include <deque>
#include <array>
#include <algorithm>
#include <iostream>
#include <iomanip>

// podio specific includes
#include "podio/ICollectionProvider.h"
#include "podio/CollectionBase.h"
#include "podio/CollectionIDTable.h"

// datamodel specific includes
#include "ExampleMCData.h"
#include "ExampleMC.h"
#include "ExampleMCObj.h"


typedef std::vector<ExampleMCData> ExampleMCDataContainer;
typedef std::deque<ExampleMCObj*> ExampleMCObjPointerContainer;

class ExampleMCCollectionIterator {

  public:
    ExampleMCCollectionIterator(int index, const ExampleMCObjPointerContainer* collection) : m_index(index), m_object(nullptr), m_collection(collection) {}

    bool operator!=(const ExampleMCCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const ExampleMC operator*() const;
    const ExampleMC* operator->() const;
    const ExampleMCCollectionIterator& operator++() const;

  private:
    mutable int m_index;
    mutable ExampleMC m_object;
    const ExampleMCObjPointerContainer* m_collection;
};

/**
A Collection is identified by an ID.
*/

class ExampleMCCollection : public podio::CollectionBase {

public:
  typedef const ExampleMCCollectionIterator const_iterator;

  ExampleMCCollection();
//  ExampleMCCollection(const ExampleMCCollection& ) = delete; // deletion doesn't work w/ ROOT IO ! :-(
//  ExampleMCCollection(ExampleMCVector* data, int collectionID);
  ~ExampleMCCollection();


  /// operator to allow pointer like calling of members a la LCIO  \n     
  ExampleMCCollection* operator->() { return (ExampleMCCollection*) this ; }


  void clear();
  /// Append a new object to the collection, and return this object.
  ExampleMC create();

  /// Append a new object to the collection, and return this object.
  /// Initialized with the parameters given
  template<typename... Args>
  ExampleMC create(Args&&... args);
  int size() const;

  /// Returns the object of given index
  const ExampleMC operator[](unsigned int index) const;
  /// Returns the object of given index
  const ExampleMC at(unsigned int index) const;


  /// Append object to the collection
  void push_back(ConstExampleMC object);

  void prepareForWrite();
  void prepareAfterRead();
  void setBuffer(void* address);
  bool setReferences(const podio::ICollectionProvider* collectionProvider);

  podio::CollRefCollection* referenceCollections() { return &m_refCollections;};

  void setID(unsigned ID){
    m_collectionID = ID;
    std::for_each(m_entries.begin(),m_entries.end(),
                 [ID](ExampleMCObj* obj){obj->id = {obj->id.index,static_cast<int>(ID)}; }
    );
  };

  bool isValid() const {
    return m_isValid;
  }

  // support for the iterator protocol
  const const_iterator begin() const {
    return const_iterator(0, &m_entries);
  }
  const const_iterator end() const {
    return const_iterator(m_entries.size(), &m_entries);
  }

  /// returns the address of the pointer to the data buffer
  void* getBufferAddress() { return (void*)&m_data;};

  /// returns the pointer to the data buffer
  std::vector<ExampleMCData>* _getBuffer() { return m_data;};

    template<size_t arraysize>
  const std::array<double,arraysize> energy() const;
  template<size_t arraysize>
  const std::array<int,arraysize> PDG() const;


private:
  bool m_isValid;
  int m_collectionID;
  ExampleMCObjPointerContainer m_entries;
  // members to handle 1-to-N-relations
  std::vector<::ConstExampleMC>* m_rel_parents; ///< Relation buffer for read / write
  std::vector<std::vector<::ConstExampleMC>*> m_rel_parents_tmp; ///< Relation buffer for internal book-keeping
  std::vector<::ConstExampleMC>* m_rel_daughters; ///< Relation buffer for read / write
  std::vector<std::vector<::ConstExampleMC>*> m_rel_daughters_tmp; ///< Relation buffer for internal book-keeping

  // members to handle streaming
  podio::CollRefCollection m_refCollections;
  ExampleMCDataContainer* m_data;
};

std::ostream& operator<<( std::ostream& o,const ExampleMCCollection& v);


template<typename... Args>
ExampleMC  ExampleMCCollection::create(Args&&... args){
  int size = m_entries.size();
  auto obj = new ExampleMCObj({size,m_collectionID},{args...});
  m_entries.push_back(obj);
  return ExampleMC(obj);
}

template<size_t arraysize>
const std::array<double,arraysize> ExampleMCCollection::energy() const {
  std::array<double,arraysize> tmp;
  auto valid_size = std::min(arraysize,m_entries.size());
  for (unsigned i = 0; i<valid_size; ++i){
    tmp[i] = m_entries[i]->data.energy;
 }
 return tmp;
}
template<size_t arraysize>
const std::array<int,arraysize> ExampleMCCollection::PDG() const {
  std::array<int,arraysize> tmp;
  auto valid_size = std::min(arraysize,m_entries.size());
  for (unsigned i = 0; i<valid_size; ++i){
    tmp[i] = m_entries[i]->data.PDG;
 }
 return tmp;
}


#endif
