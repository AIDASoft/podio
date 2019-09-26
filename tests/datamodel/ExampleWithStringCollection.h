//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef ExampleWithStringCollection_H
#define  ExampleWithStringCollection_H

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
#include "ExampleWithStringData.h"
#include "ExampleWithString.h"
#include "ExampleWithStringObj.h"


typedef std::vector<ExampleWithStringData> ExampleWithStringDataContainer;
typedef std::deque<ExampleWithStringObj*> ExampleWithStringObjPointerContainer;

class ExampleWithStringCollectionIterator {

  public:
    ExampleWithStringCollectionIterator(int index, const ExampleWithStringObjPointerContainer* collection) : m_index(index), m_object(nullptr), m_collection(collection) {}

    bool operator!=(const ExampleWithStringCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const ExampleWithString operator*() const;
    const ExampleWithString* operator->() const;
    const ExampleWithStringCollectionIterator& operator++() const;

  private:
    mutable int m_index;
    mutable ExampleWithString m_object;
    const ExampleWithStringObjPointerContainer* m_collection;
};

/**
A Collection is identified by an ID.
*/

class ExampleWithStringCollection : public podio::CollectionBase {

public:
  typedef const ExampleWithStringCollectionIterator const_iterator;

  ExampleWithStringCollection();
//  ExampleWithStringCollection(const ExampleWithStringCollection& ) = delete; // deletion doesn't work w/ ROOT IO ! :-(
//  ExampleWithStringCollection(ExampleWithStringVector* data, int collectionID);
  ~ExampleWithStringCollection();

  void clear() override final;

  /// operator to allow pointer like calling of members a la LCIO  \n     
  ExampleWithStringCollection* operator->() { return (ExampleWithStringCollection*) this ; }

  /// Append a new object to the collection, and return this object.
  ExampleWithString create();

  /// Append a new object to the collection, and return this object.
  /// Initialized with the parameters given
  template<typename... Args>
  ExampleWithString create(Args&&... args);

  /// number of elements in the collection
  int size() const override final ;

  /// fully qualified type name of elements - with namespace
  std::string getValueTypeName() const override { return std::string("ExampleWithString") ; } ;

  /// Returns the const object of given index
  const ExampleWithString operator[](unsigned int index) const;
  /// Returns the object of a given index
  ExampleWithString operator[](unsigned int index);
  /// Returns the const object of given index
  const ExampleWithString at(unsigned int index) const;
  /// Returns the object of given index
  ExampleWithString at(unsigned int index);


  /// Append object to the collection
  void push_back(ConstExampleWithString object);

  void prepareForWrite() override final;
  void prepareAfterRead() override final;
  void setBuffer(void* address) override final;
  bool setReferences(const podio::ICollectionProvider* collectionProvider) override final;

  podio::CollRefCollection* referenceCollections() override final { return &m_refCollections;};

  podio::VectorMembersInfo* vectorMembers() override {return &m_vecmem_info ; }

  void setID(unsigned ID) override final {
    m_collectionID = ID;
    std::for_each(m_entries.begin(),m_entries.end(),
                 [ID](ExampleWithStringObj* obj){obj->id = {obj->id.index,static_cast<int>(ID)}; }
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
  std::vector<ExampleWithStringData>* _getBuffer() { return m_data;};

    template<size_t arraysize>
  const std::array<std::string,arraysize> theString() const;


private:
  bool m_isValid;
  int m_collectionID;
  ExampleWithStringObjPointerContainer m_entries;
  // members to handle 1-to-N-relations

  //members to handle vector members

  // members to handle streaming
  podio::CollRefCollection m_refCollections;
  podio::VectorMembersInfo m_vecmem_info ;
  ExampleWithStringDataContainer* m_data;
};

std::ostream& operator<<( std::ostream& o,const ExampleWithStringCollection& v);


template<typename... Args>
ExampleWithString  ExampleWithStringCollection::create(Args&&... args){
  int size = m_entries.size();
  auto obj = new ExampleWithStringObj({size,m_collectionID},{args...});
  m_entries.push_back(obj);
  return ExampleWithString(obj);
}

template<size_t arraysize>
const std::array<std::string,arraysize> ExampleWithStringCollection::theString() const {
  std::array<std::string,arraysize> tmp;
  auto valid_size = std::min(arraysize,m_entries.size());
  for (unsigned i = 0; i<valid_size; ++i){
    tmp[i] = m_entries[i]->data.theString;
 }
 return tmp;
}


#endif
