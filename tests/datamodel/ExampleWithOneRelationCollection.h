//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef ExampleWithOneRelationCollection_H
#define  ExampleWithOneRelationCollection_H

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
#include "ExampleWithOneRelationData.h"
#include "ExampleWithOneRelation.h"
#include "ExampleWithOneRelationObj.h"


typedef std::vector<ExampleWithOneRelationData> ExampleWithOneRelationDataContainer;
typedef std::deque<ExampleWithOneRelationObj*> ExampleWithOneRelationObjPointerContainer;

class ExampleWithOneRelationCollectionIterator {

  public:
    ExampleWithOneRelationCollectionIterator(int index, const ExampleWithOneRelationObjPointerContainer* collection) : m_index(index), m_object(nullptr), m_collection(collection) {}

    bool operator!=(const ExampleWithOneRelationCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const ExampleWithOneRelation operator*() const;
    const ExampleWithOneRelation* operator->() const;
    const ExampleWithOneRelationCollectionIterator& operator++() const;

  private:
    mutable int m_index;
    mutable ExampleWithOneRelation m_object;
    const ExampleWithOneRelationObjPointerContainer* m_collection;
};

/**
A Collection is identified by an ID.
*/

class ExampleWithOneRelationCollection : public podio::CollectionBase {

public:
  typedef const ExampleWithOneRelationCollectionIterator const_iterator;

  ExampleWithOneRelationCollection();
//  ExampleWithOneRelationCollection(const ExampleWithOneRelationCollection& ) = delete; // deletion doesn't work w/ ROOT IO ! :-(
//  ExampleWithOneRelationCollection(ExampleWithOneRelationVector* data, int collectionID);
  ~ExampleWithOneRelationCollection();

  void clear() override;

  /// operator to allow pointer like calling of members a la LCIO  \n     
  ExampleWithOneRelationCollection* operator->() { return (ExampleWithOneRelationCollection*) this ; }

  /// Append a new object to the collection, and return this object.
  ExampleWithOneRelation create();

  /// Append a new object to the collection, and return this object.
  /// Initialized with the parameters given
  template<typename... Args>
  ExampleWithOneRelation create(Args&&... args);
  int size() const;

  /// Returns the const object of given index
  const ExampleWithOneRelation operator[](unsigned int index) const;
  /// Returns the object of a given index
  ExampleWithOneRelation operator[](unsigned int index);
  /// Returns the const object of given index
  const ExampleWithOneRelation at(unsigned int index) const;
  /// Returns the object of given index
  ExampleWithOneRelation at(unsigned int index);


  /// Append object to the collection
  void push_back(ConstExampleWithOneRelation object);

  void prepareForWrite() override;
  void prepareAfterRead() override;
  void setBuffer(void* address) override;
  bool setReferences(const podio::ICollectionProvider* collectionProvider) override;

  podio::CollRefCollection* referenceCollections() override { return &m_refCollections;};

  void setID(unsigned ID) override {
    m_collectionID = ID;
    std::for_each(m_entries.begin(),m_entries.end(),
                 [ID](ExampleWithOneRelationObj* obj){obj->id = {obj->id.index,static_cast<int>(ID)}; }
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
  std::vector<ExampleWithOneRelationData>* _getBuffer() { return m_data;};

   

private:
  bool m_isValid;
  int m_collectionID;
  ExampleWithOneRelationObjPointerContainer m_entries;
  // members to handle 1-to-N-relations
  std::vector<::ConstExampleCluster>* m_rel_cluster; ///< Relation buffer for read / write

  // members to handle streaming
  podio::CollRefCollection m_refCollections;
  ExampleWithOneRelationDataContainer* m_data;
};

std::ostream& operator<<( std::ostream& o,const ExampleWithOneRelationCollection& v);


template<typename... Args>
ExampleWithOneRelation  ExampleWithOneRelationCollection::create(Args&&... args){
  int size = m_entries.size();
  auto obj = new ExampleWithOneRelationObj({size,m_collectionID},{args...});
  m_entries.push_back(obj);
  return ExampleWithOneRelation(obj);
}



#endif
