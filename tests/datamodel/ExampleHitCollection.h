//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef ExampleHitCollection_H
#define  ExampleHitCollection_H

#include <string>
#include <vector>
#include <deque>
#include <array>

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
  ~ExampleHitCollection(){};

  void clear();
  /// Append a new object to the collection, and return this object.
  ExampleHit create();

  /// Append a new object to the collection, and return this object.
  /// Initialized with the parameters given
  template<typename... Args>
  ExampleHit create(Args&&... args);
  int size() const;

  /// Returns the object of given index
  const ExampleHit operator[](unsigned int index) const;
  /// Returns the object of given index
  const ExampleHit at(unsigned int index) const;


  /// Append object to the collection
  void push_back(ConstExampleHit object);

  void prepareForWrite();
  void prepareAfterRead();
  void setBuffer(void* address);
  bool setReferences(const podio::ICollectionProvider* collectionProvider);

  podio::CollRefCollection* referenceCollections() { return m_refCollections;};

  void setID(unsigned ID){m_collectionID = ID;};

  // support for the iterator protocol
  const const_iterator begin() const {
    return const_iterator(0, &m_entries);
  }
  const	const_iterator end() const {
    return const_iterator(m_entries.size(), &m_entries);
  }

  /// returns the address of the pointer to the data buffer
  void* getBufferAddress() { return (void*)&m_data;};

  /// returns the pointer to the data buffer
  std::vector<ExampleHitData>* _getBuffer() { return m_data;};

     template<size_t arraysize>  
  const std::array<double,arraysize> x() const;
  template<size_t arraysize>  
  const std::array<double,arraysize> y() const;
  template<size_t arraysize>  
  const std::array<double,arraysize> z() const;
  template<size_t arraysize>  
  const std::array<double,arraysize> energy() const;


private:
  int m_collectionID;
  ExampleHitObjPointerContainer m_entries;
  // members to handle 1-to-N-relations

  // members to handle streaming
  podio::CollRefCollection* m_refCollections;
  ExampleHitDataContainer* m_data;
};

template<typename... Args>
ExampleHit  ExampleHitCollection::create(Args&&... args){
  int size = m_entries.size();
  auto obj = new ExampleHitObj({size,m_collectionID},{args...});
  m_entries.push_back(obj);
  return ExampleHit(obj);
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
