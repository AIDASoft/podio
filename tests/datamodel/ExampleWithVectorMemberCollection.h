//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef ExampleWithVectorMemberCollection_H
#define  ExampleWithVectorMemberCollection_H

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
#include "ExampleWithVectorMemberData.h"
#include "ExampleWithVectorMember.h"
#include "ExampleWithVectorMemberObj.h"


typedef std::vector<ExampleWithVectorMemberData> ExampleWithVectorMemberDataContainer;
typedef std::deque<ExampleWithVectorMemberObj*> ExampleWithVectorMemberObjPointerContainer;

class ExampleWithVectorMemberCollectionIterator {

  public:
    ExampleWithVectorMemberCollectionIterator(int index, const ExampleWithVectorMemberObjPointerContainer* collection) : m_index(index), m_object(nullptr), m_collection(collection) {}

    bool operator!=(const ExampleWithVectorMemberCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const ExampleWithVectorMember operator*() const;
    const ExampleWithVectorMember* operator->() const;
    const ExampleWithVectorMemberCollectionIterator& operator++() const;

  private:
    mutable int m_index;
    mutable ExampleWithVectorMember m_object;
    const ExampleWithVectorMemberObjPointerContainer* m_collection;
};

/**
A Collection is identified by an ID.
*/

class ExampleWithVectorMemberCollection : public podio::CollectionBase {

public:
  typedef const ExampleWithVectorMemberCollectionIterator const_iterator;

  ExampleWithVectorMemberCollection();
//  ExampleWithVectorMemberCollection(const ExampleWithVectorMemberCollection& ) = delete; // deletion doesn't work w/ ROOT IO ! :-(
//  ExampleWithVectorMemberCollection(ExampleWithVectorMemberVector* data, int collectionID);
  ~ExampleWithVectorMemberCollection();

  void clear() override final;

  /// operator to allow pointer like calling of members a la LCIO  \n     
  ExampleWithVectorMemberCollection* operator->() { return (ExampleWithVectorMemberCollection*) this ; }

  /// Append a new object to the collection, and return this object.
  ExampleWithVectorMember create();

  /// Append a new object to the collection, and return this object.
  /// Initialized with the parameters given
  template<typename... Args>
  ExampleWithVectorMember create(Args&&... args);

  /// number of elements in the collection
  int size() const override final ;

  /// fully qualified type name of elements - with namespace
  std::string getValueTypeName() const override { return std::string("ExampleWithVectorMember") ; } ;

  /// Returns the const object of given index
  const ExampleWithVectorMember operator[](unsigned int index) const;
  /// Returns the object of a given index
  ExampleWithVectorMember operator[](unsigned int index);
  /// Returns the const object of given index
  const ExampleWithVectorMember at(unsigned int index) const;
  /// Returns the object of given index
  ExampleWithVectorMember at(unsigned int index);


  /// Append object to the collection
  void push_back(ConstExampleWithVectorMember object);

  void prepareForWrite() override final;
  void prepareAfterRead() override final;
  void setBuffer(void* address) override final;
  bool setReferences(const podio::ICollectionProvider* collectionProvider) override final;

  podio::CollRefCollection* referenceCollections() override final { return &m_refCollections;};

  podio::VectorMembersInfo* vectorMembers() override {return &m_vecmem_info ; }

  void setID(unsigned ID) override final {
    m_collectionID = ID;
    std::for_each(m_entries.begin(),m_entries.end(),
                 [ID](ExampleWithVectorMemberObj* obj){obj->id = {obj->id.index,static_cast<int>(ID)}; }
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
  std::vector<ExampleWithVectorMemberData>* _getBuffer() { return m_data;};

   

private:
  bool m_isValid;
  int m_collectionID;
  ExampleWithVectorMemberObjPointerContainer m_entries;
  // members to handle 1-to-N-relations

  //members to handle vector members
  std::vector<int>* m_vec_count; /// combined vector of all objects in collection
  std::vector<std::vector<int>*> m_vecs_count; /// pointers to individual member vectors

  // members to handle streaming
  podio::CollRefCollection m_refCollections;
  podio::VectorMembersInfo m_vecmem_info ;
  ExampleWithVectorMemberDataContainer* m_data;
};

std::ostream& operator<<( std::ostream& o,const ExampleWithVectorMemberCollection& v);


template<typename... Args>
ExampleWithVectorMember  ExampleWithVectorMemberCollection::create(Args&&... args){
  int size = m_entries.size();
  auto obj = new ExampleWithVectorMemberObj({size,m_collectionID},{args...});
  m_entries.push_back(obj);
  return ExampleWithVectorMember(obj);
}



#endif
