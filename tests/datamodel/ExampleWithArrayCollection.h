//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef ExampleWithArrayCollection_H
#define  ExampleWithArrayCollection_H

#include <string>
#include <vector>
#include <deque>
#include <array>
#include <algorithm>

// podio specific includes
#include "podio/ICollectionProvider.h"
#include "podio/CollectionBase.h"
#include "podio/CollectionIDTable.h"

// datamodel specific includes
#include "ExampleWithArrayData.h"
#include "ExampleWithArray.h"
#include "ExampleWithArrayObj.h"


typedef std::vector<ExampleWithArrayData> ExampleWithArrayDataContainer;
typedef std::deque<ExampleWithArrayObj*> ExampleWithArrayObjPointerContainer;

class ExampleWithArrayCollectionIterator {

  public:
    ExampleWithArrayCollectionIterator(int index, const ExampleWithArrayObjPointerContainer* collection) : m_index(index), m_object(nullptr), m_collection(collection) {}

    bool operator!=(const ExampleWithArrayCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const ExampleWithArray operator*() const;
    const ExampleWithArray* operator->() const;
    const ExampleWithArrayCollectionIterator& operator++() const;

  private:
    mutable int m_index;
    mutable ExampleWithArray m_object;
    const ExampleWithArrayObjPointerContainer* m_collection;
};

/**
A Collection is identified by an ID.
*/

class ExampleWithArrayCollection : public podio::CollectionBase {

public:
  typedef const ExampleWithArrayCollectionIterator const_iterator;

  ExampleWithArrayCollection();
//  ExampleWithArrayCollection(const ExampleWithArrayCollection& ) = delete; // deletion doesn't work w/ ROOT IO ! :-(
//  ExampleWithArrayCollection(ExampleWithArrayVector* data, int collectionID);
  ~ExampleWithArrayCollection();

  void clear() override;
  /// Append a new object to the collection, and return this object.
  ExampleWithArray create();

  /// Append a new object to the collection, and return this object.
  /// Initialized with the parameters given
  template<typename... Args>
  ExampleWithArray create(Args&&... args);
  int size() const;

  /// Returns the const object of given index
  const ExampleWithArray operator[](unsigned int index) const;
  /// Returns the object of a given index
  ExampleWithArray operator[](unsigned int index);
  /// Returns the const object of given index
  const ExampleWithArray at(unsigned int index) const;
  /// Returns the object of given index
  ExampleWithArray at(unsigned int index);


  /// Append object to the collection
  void push_back(ConstExampleWithArray object);

  void prepareForWrite() override;
  void prepareAfterRead() override;
  void setBuffer(void* address) override;
  bool setReferences(const podio::ICollectionProvider* collectionProvider) override;

  podio::CollRefCollection* referenceCollections() override { return &m_refCollections;};

  void setID(unsigned ID) override {
    m_collectionID = ID;
    std::for_each(m_entries.begin(),m_entries.end(),
                 [ID](ExampleWithArrayObj* obj){obj->id = {obj->id.index,static_cast<int>(ID)}; }
    );
  };

  bool isValid() const override {
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
  void* getBufferAddress() override { return (void*)&m_data;};

  /// returns the pointer to the data buffer
  std::vector<ExampleWithArrayData>* _getBuffer() { return m_data;};

    template<size_t arraysize>
  const std::array<NotSoSimpleStruct,arraysize> arrayStruct() const;
  template<size_t arraysize>
  const std::array<std::array<int, 4>,arraysize> myArray() const;
  template<size_t arraysize>
  const std::array<std::array<int, 4>,arraysize> anotherArray2() const;
  template<size_t arraysize>
  const std::array<std::array<int, 4>,arraysize> snail_case_array() const;
  template<size_t arraysize>
  const std::array<std::array<int, 4>,arraysize> snail_case_Array3() const;
  template<size_t arraysize>
  const std::array<std::array<ex2::NamespaceStruct, 4>,arraysize> structArray() const;


private:
  bool m_isValid;
  int m_collectionID;
  ExampleWithArrayObjPointerContainer m_entries;
  // members to handle 1-to-N-relations

  // members to handle streaming
  podio::CollRefCollection m_refCollections;
  ExampleWithArrayDataContainer* m_data;
};

template<typename... Args>
ExampleWithArray  ExampleWithArrayCollection::create(Args&&... args){
  int size = m_entries.size();
  auto obj = new ExampleWithArrayObj({size,m_collectionID},{args...});
  m_entries.push_back(obj);
  return ExampleWithArray(obj);
}

template<size_t arraysize>
const std::array<class NotSoSimpleStruct,arraysize> ExampleWithArrayCollection::arrayStruct() const {
  std::array<class NotSoSimpleStruct,arraysize> tmp;
  auto valid_size = std::min(arraysize,m_entries.size());
  for (unsigned i = 0; i<valid_size; ++i){
    tmp[i] = m_entries[i]->data.arrayStruct;
 }
 return tmp;
}
template<size_t arraysize>
const std::array<class std::array<int, 4>,arraysize> ExampleWithArrayCollection::myArray() const {
  std::array<class std::array<int, 4>,arraysize> tmp;
  auto valid_size = std::min(arraysize,m_entries.size());
  for (unsigned i = 0; i<valid_size; ++i){
    tmp[i] = m_entries[i]->data.myArray;
 }
 return tmp;
}
template<size_t arraysize>
const std::array<class std::array<int, 4>,arraysize> ExampleWithArrayCollection::anotherArray2() const {
  std::array<class std::array<int, 4>,arraysize> tmp;
  auto valid_size = std::min(arraysize,m_entries.size());
  for (unsigned i = 0; i<valid_size; ++i){
    tmp[i] = m_entries[i]->data.anotherArray2;
 }
 return tmp;
}
template<size_t arraysize>
const std::array<class std::array<int, 4>,arraysize> ExampleWithArrayCollection::snail_case_array() const {
  std::array<class std::array<int, 4>,arraysize> tmp;
  auto valid_size = std::min(arraysize,m_entries.size());
  for (unsigned i = 0; i<valid_size; ++i){
    tmp[i] = m_entries[i]->data.snail_case_array;
 }
 return tmp;
}
template<size_t arraysize>
const std::array<class std::array<int, 4>,arraysize> ExampleWithArrayCollection::snail_case_Array3() const {
  std::array<class std::array<int, 4>,arraysize> tmp;
  auto valid_size = std::min(arraysize,m_entries.size());
  for (unsigned i = 0; i<valid_size; ++i){
    tmp[i] = m_entries[i]->data.snail_case_Array3;
 }
 return tmp;
}
template<size_t arraysize>
const std::array<class std::array<ex2::NamespaceStruct, 4>,arraysize> ExampleWithArrayCollection::structArray() const {
  std::array<class std::array<ex2::NamespaceStruct, 4>,arraysize> tmp;
  auto valid_size = std::min(arraysize,m_entries.size());
  for (unsigned i = 0; i<valid_size; ++i){
    tmp[i] = m_entries[i]->data.structArray;
 }
 return tmp;
}


#endif
