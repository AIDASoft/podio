//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef ExampleForCyclicDependency1Collection_H
#define  ExampleForCyclicDependency1Collection_H

#include <string>
#include <vector>
#include <deque>
#include <array>

// podio specific includes
#include "podio/ICollectionProvider.h"
#include "podio/CollectionBase.h"
#include "podio/CollectionIDTable.h"

// datamodel specific includes
#include "ExampleForCyclicDependency1Data.h"
#include "ExampleForCyclicDependency1.h"
#include "ExampleForCyclicDependency1Obj.h"


typedef std::vector<ExampleForCyclicDependency1Data> ExampleForCyclicDependency1DataContainer;
typedef std::deque<ExampleForCyclicDependency1Obj*> ExampleForCyclicDependency1ObjPointerContainer;

class ExampleForCyclicDependency1CollectionIterator {

  public:
    ExampleForCyclicDependency1CollectionIterator(int index, const ExampleForCyclicDependency1ObjPointerContainer* collection) : m_index(index), m_object(nullptr), m_collection(collection) {}

    bool operator!=(const ExampleForCyclicDependency1CollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const ExampleForCyclicDependency1 operator*() const;
    const ExampleForCyclicDependency1* operator->() const;
    const ExampleForCyclicDependency1CollectionIterator& operator++() const;

  private:
    mutable int m_index;
    mutable ExampleForCyclicDependency1 m_object;
    const ExampleForCyclicDependency1ObjPointerContainer* m_collection;
};

/**
A Collection is identified by an ID.
*/

class ExampleForCyclicDependency1Collection : public podio::CollectionBase {

public:
  typedef const ExampleForCyclicDependency1CollectionIterator const_iterator;

  ExampleForCyclicDependency1Collection();
//  ExampleForCyclicDependency1Collection(const ExampleForCyclicDependency1Collection& ) = delete; // deletion doesn't work w/ ROOT IO ! :-(
//  ExampleForCyclicDependency1Collection(ExampleForCyclicDependency1Vector* data, int collectionID);
  ~ExampleForCyclicDependency1Collection(){};

  void clear();
  /// Append a new object to the collection, and return this object.
  ExampleForCyclicDependency1 create();

  /// Append a new object to the collection, and return this object.
  /// Initialized with the parameters given
  template<typename... Args>
  ExampleForCyclicDependency1 create(Args&&... args);
  int size() const;

  /// Returns the object of given index
  const ExampleForCyclicDependency1 operator[](unsigned int index) const;
  /// Returns the object of given index
  const ExampleForCyclicDependency1 at(unsigned int index) const;


  /// Append object to the collection
  void push_back(ConstExampleForCyclicDependency1 object);

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
  std::vector<ExampleForCyclicDependency1Data>* _getBuffer() { return m_data;};

   

private:
  int m_collectionID;
  ExampleForCyclicDependency1ObjPointerContainer m_entries;
  // members to handle 1-to-N-relations
  std::vector<ConstExampleForCyclicDependency2>* m_rel_ref; //relation buffer for r/w

  // members to handle streaming
  podio::CollRefCollection* m_refCollections;
  ExampleForCyclicDependency1DataContainer* m_data;
};

template<typename... Args>
ExampleForCyclicDependency1  ExampleForCyclicDependency1Collection::create(Args&&... args){
  int size = m_entries.size();
  auto obj = new ExampleForCyclicDependency1Obj({size,m_collectionID},{args...});
  m_entries.push_back(obj);
  return ExampleForCyclicDependency1(obj);
}



#endif
