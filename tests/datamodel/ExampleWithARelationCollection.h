//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef ExampleWithARelationCollection_H
#define  ExampleWithARelationCollection_H

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
#include "ExampleWithARelationData.h"
#include "ExampleWithARelation.h"
#include "ExampleWithARelationObj.h"

namespace ex42 {
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
  ~ExampleWithARelationCollection();

  void clear() override final;

  /// operator to allow pointer like calling of members a la LCIO  \n     
  ExampleWithARelationCollection* operator->() { return (ExampleWithARelationCollection*) this ; }

  /// Append a new object to the collection, and return this object.
  ExampleWithARelation create();

  /// Append a new object to the collection, and return this object.
  /// Initialized with the parameters given
  template<typename... Args>
  ExampleWithARelation create(Args&&... args);

  /// number of elements in the collection
  int size() const override final ;

  /// fully qualified type name of elements - with namespace
  std::string getValueTypeName() const override { return std::string("ex42::ExampleWithARelation") ; } ;

  /// Returns the const object of given index
  const ExampleWithARelation operator[](unsigned int index) const;
  /// Returns the object of a given index
  ExampleWithARelation operator[](unsigned int index);
  /// Returns the const object of given index
  const ExampleWithARelation at(unsigned int index) const;
  /// Returns the object of given index
  ExampleWithARelation at(unsigned int index);


  /// Append object to the collection
  void push_back(ConstExampleWithARelation object);

  void prepareForWrite() override final;
  void prepareAfterRead() override final;
  void setBuffer(void* address) override final;
  bool setReferences(const podio::ICollectionProvider* collectionProvider) override final;

  podio::CollRefCollection* referenceCollections() override final { return &m_refCollections;};

  podio::VectorMembersInfo* vectorMembers() override {return &m_vecmem_info ; }

  void setID(unsigned ID) override final {
    m_collectionID = ID;
    std::for_each(m_entries.begin(),m_entries.end(),
                 [ID](ExampleWithARelationObj* obj){obj->id = {obj->id.index,static_cast<int>(ID)}; }
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
  std::vector<ExampleWithARelationData>* _getBuffer() { return m_data;};

    template<size_t arraysize>
  const std::array<float,arraysize> number() const;


private:
  bool m_isValid;
  int m_collectionID;
  ExampleWithARelationObjPointerContainer m_entries;
  // members to handle 1-to-N-relations
  std::vector<ex42::ConstExampleWithNamespace>* m_rel_refs; ///< Relation buffer for read / write
  std::vector<std::vector<ex42::ConstExampleWithNamespace>*> m_rel_refs_tmp; ///< Relation buffer for internal book-keeping
  std::vector<ex42::ConstExampleWithNamespace>* m_rel_ref; ///< Relation buffer for read / write

  //members to handle vector members

  // members to handle streaming
  podio::CollRefCollection m_refCollections;
  podio::VectorMembersInfo m_vecmem_info ;
  ExampleWithARelationDataContainer* m_data;
};

std::ostream& operator<<( std::ostream& o,const ExampleWithARelationCollection& v);


template<typename... Args>
ExampleWithARelation  ExampleWithARelationCollection::create(Args&&... args){
  int size = m_entries.size();
  auto obj = new ExampleWithARelationObj({size,m_collectionID},{args...});
  m_entries.push_back(obj);
  return ExampleWithARelation(obj);
}

template<size_t arraysize>
const std::array<float,arraysize> ExampleWithARelationCollection::number() const {
  std::array<float,arraysize> tmp;
  auto valid_size = std::min(arraysize,m_entries.size());
  for (unsigned i = 0; i<valid_size; ++i){
    tmp[i] = m_entries[i]->data.number;
 }
 return tmp;
}

} // namespace ex42
#endif
