//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef ExampleHitCollection_H
#define  ExampleHitCollection_H

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
#include "ExampleHitData.h"
#include "ExampleHit.h"
#include "ExampleHitObj.h"


typedef std::vector<ExampleHitData> ExampleHitDataContainer;
typedef std::deque<ExampleHitObj*> ExampleHitObjPointerContainer;

class ExampleHitCollectionIterator {

  public:
    ExampleHitCollectionIterator(int index, const ExampleHitObjPointerContainer* collection) : m_index(index), m_object(nullptr), m_collection(collection) {}

    bool operator!=(const ExampleHitCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const ExampleHit operator*() const;
    const ExampleHit* operator->() const;
    const ExampleHitCollectionIterator& operator++() const;

  private:
    mutable int m_index;
    mutable ExampleHit m_object;
    const ExampleHitObjPointerContainer* m_collection;
};

/**
A Collection is identified by an ID.
*/

class ExampleHitCollection : public podio::CollectionBase {

public:
  typedef const ExampleHitCollectionIterator const_iterator;

  ExampleHitCollection();
//  ExampleHitCollection(const ExampleHitCollection& ) = delete; // deletion doesn't work w/ ROOT IO ! :-(
//  ExampleHitCollection(ExampleHitVector* data, int collectionID);
  ~ExampleHitCollection();

  void clear() override final;

  /// operator to allow pointer like calling of members a la LCIO  \n     
  ExampleHitCollection* operator->() { return (ExampleHitCollection*) this ; }

  /// Append a new object to the collection, and return this object.
  ExampleHit create();

  /// Append a new object to the collection, and return this object.
  /// Initialized with the parameters given
  template<typename... Args>
  ExampleHit create(Args&&... args);

  /// number of elements in the collection
  int size() const override final ;

  /// fully qualified type name of elements - with namespace
  std::string getValueTypeName() const override { return std::string("ExampleHit") ; } ;

  /// Returns the const object of given index
  const ExampleHit operator[](unsigned int index) const;
  /// Returns the object of a given index
  ExampleHit operator[](unsigned int index);
  /// Returns the const object of given index
  const ExampleHit at(unsigned int index) const;
  /// Returns the object of given index
  ExampleHit at(unsigned int index);


  /// Append object to the collection
  void push_back(ConstExampleHit object);

  void prepareForWrite() override final;
  void prepareAfterRead() override final;
  void setBuffer(void* address) override final;
  bool setReferences(const podio::ICollectionProvider* collectionProvider) override final;

  podio::CollRefCollection* referenceCollections() override final { return &m_refCollections;};

  podio::VectorMembersInfo* vectorMembers() override {return &m_vecmem_info ; }

  void setID(unsigned ID) override final {
    m_collectionID = ID;
    std::for_each(m_entries.begin(),m_entries.end(),
                 [ID](ExampleHitObj* obj){obj->id = {obj->id.index,static_cast<int>(ID)}; }
    );
  };

  bool isValid() const override final {
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
  void* getBufferAddress() override final { return (void*)&m_data;};

  /// returns the pointer to the data buffer
  std::vector<ExampleHitData>* _getBuffer() { return m_data;};

    template<size_t arraysize>
  const std::array<unsigned long long,arraysize> cellID() const;
  template<size_t arraysize>
  const std::array<double,arraysize> x() const;
  template<size_t arraysize>
  const std::array<double,arraysize> y() const;
  template<size_t arraysize>
  const std::array<double,arraysize> z() const;
  template<size_t arraysize>
  const std::array<double,arraysize> energy() const;


private:
  bool m_isValid;
  int m_collectionID;
  ExampleHitObjPointerContainer m_entries;
  // members to handle 1-to-N-relations

  //members to handle vector members

  // members to handle streaming
  podio::CollRefCollection m_refCollections;
  podio::VectorMembersInfo m_vecmem_info ;
  ExampleHitDataContainer* m_data;
};

std::ostream& operator<<( std::ostream& o,const ExampleHitCollection& v);


template<typename... Args>
ExampleHit  ExampleHitCollection::create(Args&&... args){
  int size = m_entries.size();
  auto obj = new ExampleHitObj({size,m_collectionID},{args...});
  m_entries.push_back(obj);
  return ExampleHit(obj);
}

template<size_t arraysize>
const std::array<unsigned long long,arraysize> ExampleHitCollection::cellID() const {
  std::array<unsigned long long,arraysize> tmp;
  auto valid_size = std::min(arraysize,m_entries.size());
  for (unsigned i = 0; i<valid_size; ++i){
    tmp[i] = m_entries[i]->data.cellID;
 }
 return tmp;
}
template<size_t arraysize>
const std::array<double,arraysize> ExampleHitCollection::x() const {
  std::array<double,arraysize> tmp;
  auto valid_size = std::min(arraysize,m_entries.size());
  for (unsigned i = 0; i<valid_size; ++i){
    tmp[i] = m_entries[i]->data.x;
 }
 return tmp;
}
template<size_t arraysize>
const std::array<double,arraysize> ExampleHitCollection::y() const {
  std::array<double,arraysize> tmp;
  auto valid_size = std::min(arraysize,m_entries.size());
  for (unsigned i = 0; i<valid_size; ++i){
    tmp[i] = m_entries[i]->data.y;
 }
 return tmp;
}
template<size_t arraysize>
const std::array<double,arraysize> ExampleHitCollection::z() const {
  std::array<double,arraysize> tmp;
  auto valid_size = std::min(arraysize,m_entries.size());
  for (unsigned i = 0; i<valid_size; ++i){
    tmp[i] = m_entries[i]->data.z;
 }
 return tmp;
}
template<size_t arraysize>
const std::array<double,arraysize> ExampleHitCollection::energy() const {
  std::array<double,arraysize> tmp;
  auto valid_size = std::min(arraysize,m_entries.size());
  for (unsigned i = 0; i<valid_size; ++i){
    tmp[i] = m_entries[i]->data.energy;
 }
 return tmp;
}


#endif
