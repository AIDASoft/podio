//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef ExampleWithComponentCollection_H
#define  ExampleWithComponentCollection_H

#include <string>
#include <vector>
#include <deque>
#include <array>

// podio specific includes
#include "podio/ICollectionProvider.h"
#include "podio/CollectionBase.h"
#include "podio/CollectionIDTable.h"

// datamodel specific includes
#include "ExampleWithComponentData.h"
#include "ExampleWithComponent.h"
#include "ExampleWithComponentObj.h"

typedef std::vector<ExampleWithComponentData> ExampleWithComponentDataContainer;
typedef std::deque<ExampleWithComponentObj*> ExampleWithComponentObjPointerContainer;

class ExampleWithComponentCollectionIterator {

  public:
    ExampleWithComponentCollectionIterator(int index, const ExampleWithComponentObjPointerContainer* collection) : m_index(index), m_object(nullptr), m_collection(collection) {}

    bool operator!=(const ExampleWithComponentCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const ExampleWithComponent operator*() const;
    const ExampleWithComponent* operator->() const;
    const ExampleWithComponentCollectionIterator& operator++() const;

  private:
    mutable int m_index;
    mutable ExampleWithComponent m_object;
    const ExampleWithComponentObjPointerContainer* m_collection;
};

/**
A Collection is identified by an ID.
*/

class ExampleWithComponentCollection : public podio::CollectionBase {

public:
  typedef const ExampleWithComponentCollectionIterator const_iterator;

  ExampleWithComponentCollection();
//  ExampleWithComponentCollection(const ExampleWithComponentCollection& ) = delete; // deletion doesn't work w/ ROOT IO ! :-(
//  ExampleWithComponentCollection(ExampleWithComponentVector* data, int collectionID);
  ~ExampleWithComponentCollection(){};

  void clear();
  /// Append a new object to the collection, and return this object.
  ExampleWithComponent create();

  /// Append a new object to the collection, and return this object.
  /// Initialized with the parameters given
  template<typename... Args>
  ExampleWithComponent create(Args&&... args);
  int size() const;

  /// Returns the object of given index
  const ExampleWithComponent operator[](unsigned int index) const;
  /// Returns the object of given index
  const ExampleWithComponent at(unsigned int index) const;


  /// Append object to the collection
  void push_back(ConstExampleWithComponent object);

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
  std::vector<ExampleWithComponentData>* _getBuffer() { return m_data;};

     template<size_t arraysize>  
  const std::array<NotSoSimpleStruct,arraysize> component() const;


private:
  int m_collectionID;
  ExampleWithComponentObjPointerContainer m_entries;
  // members to handle 1-to-N-relations

  // members to handle streaming
  podio::CollRefCollection* m_refCollections;
  ExampleWithComponentDataContainer* m_data;
};

template<typename... Args>
ExampleWithComponent  ExampleWithComponentCollection::create(Args&&... args){
  int size = m_entries.size();
  auto obj = new ExampleWithComponentObj({size,m_collectionID},{args...});
  m_entries.push_back(obj);
  return ExampleWithComponent(obj);
}

template<size_t arraysize>
const std::array<NotSoSimpleStruct,arraysize> ExampleWithComponentCollection::component() const {
  std::array<NotSoSimpleStruct,arraysize> tmp;
  auto valid_size = std::min(arraysize,m_entries.size());
  for (unsigned i = 0; i<valid_size; ++i){
    tmp[i] = m_entries[i]->data.component;
 }
 return tmp;
}


#endif
