//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef ExampleReferencingTypeCollection_H
#define  ExampleReferencingTypeCollection_H

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
#include "ExampleReferencingTypeData.h"
#include "ExampleReferencingType.h"
#include "ExampleReferencingTypeObj.h"


typedef std::vector<ExampleReferencingTypeData> ExampleReferencingTypeDataContainer;
typedef std::deque<ExampleReferencingTypeObj*> ExampleReferencingTypeObjPointerContainer;

class ExampleReferencingTypeCollectionIterator {

  public:
    ExampleReferencingTypeCollectionIterator(int index, const ExampleReferencingTypeObjPointerContainer* collection) : m_index(index), m_object(nullptr), m_collection(collection) {}

    bool operator!=(const ExampleReferencingTypeCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const ExampleReferencingType operator*() const;
    const ExampleReferencingType* operator->() const;
    const ExampleReferencingTypeCollectionIterator& operator++() const;

  private:
    mutable int m_index;
    mutable ExampleReferencingType m_object;
    const ExampleReferencingTypeObjPointerContainer* m_collection;
};

/**
A Collection is identified by an ID.
*/

class ExampleReferencingTypeCollection : public podio::CollectionBase {

public:
  typedef const ExampleReferencingTypeCollectionIterator const_iterator;

  ExampleReferencingTypeCollection();
//  ExampleReferencingTypeCollection(const ExampleReferencingTypeCollection& ) = delete; // deletion doesn't work w/ ROOT IO ! :-(
//  ExampleReferencingTypeCollection(ExampleReferencingTypeVector* data, int collectionID);
  ~ExampleReferencingTypeCollection();

  void clear() override final;

  /// operator to allow pointer like calling of members a la LCIO  \n     
  ExampleReferencingTypeCollection* operator->() { return (ExampleReferencingTypeCollection*) this ; }

  /// Append a new object to the collection, and return this object.
  ExampleReferencingType create();

  /// Append a new object to the collection, and return this object.
  /// Initialized with the parameters given
  template<typename... Args>
  ExampleReferencingType create(Args&&... args);

  /// number of elements in the collection
  int size() const override final ;

  /// fully qualified type name of elements - with namespace
  std::string getValueTypeName() const override { return std::string("ExampleReferencingType") ; } ;

  /// Returns the const object of given index
  const ExampleReferencingType operator[](unsigned int index) const;
  /// Returns the object of a given index
  ExampleReferencingType operator[](unsigned int index);
  /// Returns the const object of given index
  const ExampleReferencingType at(unsigned int index) const;
  /// Returns the object of given index
  ExampleReferencingType at(unsigned int index);


  /// Append object to the collection
  void push_back(ConstExampleReferencingType object);

  void prepareForWrite() override final;
  void prepareAfterRead() override final;
  void setBuffer(void* address) override final;
  bool setReferences(const podio::ICollectionProvider* collectionProvider) override final;

  podio::CollRefCollection* referenceCollections() override final { return &m_refCollections;};

  podio::VectorMembersInfo* vectorMembers() override {return &m_vecmem_info ; }

  void setID(unsigned ID) override final {
    m_collectionID = ID;
    std::for_each(m_entries.begin(),m_entries.end(),
                 [ID](ExampleReferencingTypeObj* obj){obj->id = {obj->id.index,static_cast<int>(ID)}; }
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
  std::vector<ExampleReferencingTypeData>* _getBuffer() { return m_data;};

   

private:
  bool m_isValid;
  int m_collectionID;
  ExampleReferencingTypeObjPointerContainer m_entries;
  // members to handle 1-to-N-relations
  std::vector<::ConstExampleCluster>* m_rel_Clusters; ///< Relation buffer for read / write
  std::vector<std::vector<::ConstExampleCluster>*> m_rel_Clusters_tmp; ///< Relation buffer for internal book-keeping
  std::vector<::ConstExampleReferencingType>* m_rel_Refs; ///< Relation buffer for read / write
  std::vector<std::vector<::ConstExampleReferencingType>*> m_rel_Refs_tmp; ///< Relation buffer for internal book-keeping

  //members to handle vector members

  // members to handle streaming
  podio::CollRefCollection m_refCollections;
  podio::VectorMembersInfo m_vecmem_info ;
  ExampleReferencingTypeDataContainer* m_data;
};

std::ostream& operator<<( std::ostream& o,const ExampleReferencingTypeCollection& v);


template<typename... Args>
ExampleReferencingType  ExampleReferencingTypeCollection::create(Args&&... args){
  int size = m_entries.size();
  auto obj = new ExampleReferencingTypeObj({size,m_collectionID},{args...});
  m_entries.push_back(obj);
  return ExampleReferencingType(obj);
}



#endif
