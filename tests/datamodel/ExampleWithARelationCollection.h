//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef ExampleWithARelationCollection_H
#define  ExampleWithARelationCollection_H

#include <string>
#include <vector>
#include <deque>
#include <array>

// podio specific includes
#include "podio/ICollectionProvider.h"
#include "podio/CollectionBase.h"
#include "podio/CollectionIDTable.h"

// datamodel specific includes
#include "ExampleWithARelationData.h"
#include "ExampleWithARelation.h"
#include "ExampleWithARelationObj.h"

namespace ex {
typedef std::vector<ExampleWithARelationData> ExampleWithARelationDataContainer;
typedef std::deque<ExampleWithARelationObj*> ExampleWithARelationObjPointerContainer;

class ExampleWithARelationCollectionIterator {

  public:
    ExampleWithARelationCollectionIterator(int index, const ExampleWithARelationObjPointerContainer* collection) : m_index(index), m_object(nullptr), m_collection(collection) {}

    bool operator!=(const ExampleWithARelationCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const ExampleWithARelation operator*() const;
    const ExampleWithARelation* operator->() const;
    const ExampleWithARelationCollectionIterator& operator++() const;

  private:
    mutable int m_index;
    mutable ExampleWithARelation m_object;
    const ExampleWithARelationObjPointerContainer* m_collection;
};

/**
A Collection is identified by an ID.
*/

class ExampleWithARelationCollection : public podio::CollectionBase {

public:
  typedef const ExampleWithARelationCollectionIterator const_iterator;

  ExampleWithARelationCollection();
//  ExampleWithARelationCollection(const ExampleWithARelationCollection& ) = delete; // deletion doesn't work w/ ROOT IO ! :-(
//  ExampleWithARelationCollection(ExampleWithARelationVector* data, int collectionID);
  ~ExampleWithARelationCollection(){};

  void clear();
  /// Append a new object to the collection, and return this object.
  ExampleWithARelation create();

  /// Append a new object to the collection, and return this object.
  /// Initialized with the parameters given
  template<typename... Args>
  ExampleWithARelation create(Args&&... args);
  int size() const;

  /// Returns the object of given index
  const ExampleWithARelation operator[](unsigned int index) const;
  /// Returns the object of given index
  const ExampleWithARelation at(unsigned int index) const;


  /// Append object to the collection
  void push_back(ConstExampleWithARelation object);

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
  std::vector<ExampleWithARelationData>* _getBuffer() { return m_data;};

   

private:
  int m_collectionID;
  ExampleWithARelationObjPointerContainer m_entries;
  // members to handle 1-to-N-relations
  std::vector<ex::ConstExampleWithNamespace>* m_rel_ref; //relation buffer for r/w

  // members to handle streaming
  podio::CollRefCollection* m_refCollections;
  ExampleWithARelationDataContainer* m_data;
};

template<typename... Args>
ExampleWithARelation  ExampleWithARelationCollection::create(Args&&... args){
  int size = m_entries.size();
  auto obj = new ExampleWithARelationObj({size,m_collectionID},{args...});
  m_entries.push_back(obj);
  return ExampleWithARelation(obj);
}


} // namespace ex
#endif
