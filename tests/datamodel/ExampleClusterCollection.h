//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef ExampleClusterCollection_H
#define  ExampleClusterCollection_H

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
#include "ExampleClusterData.h"
#include "ExampleCluster.h"
#include "ExampleClusterObj.h"


typedef std::vector<ExampleClusterData> ExampleClusterDataContainer;
typedef std::deque<ExampleClusterObj*> ExampleClusterObjPointerContainer;

class ExampleClusterCollectionIterator {

  public:
    ExampleClusterCollectionIterator(int index, const ExampleClusterObjPointerContainer* collection) : m_index(index), m_object(nullptr), m_collection(collection) {}

    bool operator!=(const ExampleClusterCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const ExampleCluster operator*() const;
    const ExampleCluster* operator->() const;
    const ExampleClusterCollectionIterator& operator++() const;

  private:
    mutable int m_index;
    mutable ExampleCluster m_object;
    const ExampleClusterObjPointerContainer* m_collection;
};

/**
A Collection is identified by an ID.
*/

class ExampleClusterCollection : public podio::CollectionBase {

public:
  typedef const ExampleClusterCollectionIterator const_iterator;

  ExampleClusterCollection();
//  ExampleClusterCollection(const ExampleClusterCollection& ) = delete; // deletion doesn't work w/ ROOT IO ! :-(
//  ExampleClusterCollection(ExampleClusterVector* data, int collectionID);
  ~ExampleClusterCollection();

  void clear() override;

  /// operator to allow pointer like calling of members a la LCIO  \n     
  ExampleClusterCollection* operator->() { return (ExampleClusterCollection*) this ; }

  /// Append a new object to the collection, and return this object.
  ExampleCluster create();

  /// Append a new object to the collection, and return this object.
  /// Initialized with the parameters given
  template<typename... Args>
  ExampleCluster create(Args&&... args);
  int size() const;

  /// Returns the const object of given index
  const ExampleCluster operator[](unsigned int index) const;
  /// Returns the object of a given index
  ExampleCluster operator[](unsigned int index);
  /// Returns the const object of given index
  const ExampleCluster at(unsigned int index) const;
  /// Returns the object of given index
  ExampleCluster at(unsigned int index);


  /// Append object to the collection
  void push_back(ConstExampleCluster object);

  void prepareForWrite() override;
  void prepareAfterRead() override;
  void setBuffer(void* address) override;
  bool setReferences(const podio::ICollectionProvider* collectionProvider) override;

  podio::CollRefCollection* referenceCollections() override { return &m_refCollections;};

  void setID(unsigned ID) override {
    m_collectionID = ID;
    std::for_each(m_entries.begin(),m_entries.end(),
                 [ID](ExampleClusterObj* obj){obj->id = {obj->id.index,static_cast<int>(ID)}; }
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
  std::vector<ExampleClusterData>* _getBuffer() { return m_data;};

    template<size_t arraysize>
  const std::array<double,arraysize> energy() const;


private:
  bool m_isValid;
  int m_collectionID;
  ExampleClusterObjPointerContainer m_entries;
  // members to handle 1-to-N-relations
  std::vector<::ConstExampleHit>* m_rel_Hits; ///< Relation buffer for read / write
  std::vector<std::vector<::ConstExampleHit>*> m_rel_Hits_tmp; ///< Relation buffer for internal book-keeping
  std::vector<::ConstExampleCluster>* m_rel_Clusters; ///< Relation buffer for read / write
  std::vector<std::vector<::ConstExampleCluster>*> m_rel_Clusters_tmp; ///< Relation buffer for internal book-keeping

  // members to handle streaming
  podio::CollRefCollection m_refCollections;
  ExampleClusterDataContainer* m_data;
};

std::ostream& operator<<( std::ostream& o,const ExampleClusterCollection& v);


template<typename... Args>
ExampleCluster  ExampleClusterCollection::create(Args&&... args){
  int size = m_entries.size();
  auto obj = new ExampleClusterObj({size,m_collectionID},{args...});
  m_entries.push_back(obj);
  return ExampleCluster(obj);
}

template<size_t arraysize>
const std::array<double,arraysize> ExampleClusterCollection::energy() const {
  std::array<double,arraysize> tmp;
  auto valid_size = std::min(arraysize,m_entries.size());
  for (unsigned i = 0; i<valid_size; ++i){
    tmp[i] = m_entries[i]->data.energy;
 }
 return tmp;
}


#endif
