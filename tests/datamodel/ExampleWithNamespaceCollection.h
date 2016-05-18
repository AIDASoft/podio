//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef ExampleWithNamespaceCollection_H
#define  ExampleWithNamespaceCollection_H

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
#include "ExampleWithNamespaceData.h"
#include "ExampleWithNamespace.h"
#include "ExampleWithNamespaceObj.h"

namespace ex {
typedef std::vector<ExampleWithNamespaceData> ExampleWithNamespaceDataContainer;
typedef std::deque<ExampleWithNamespaceObj*> ExampleWithNamespaceObjPointerContainer;

class ExampleWithNamespaceCollectionIterator {

  public:
    ExampleWithNamespaceCollectionIterator(int index, const ExampleWithNamespaceObjPointerContainer* collection) : m_index(index), m_object(nullptr), m_collection(collection) {}

    bool operator!=(const ExampleWithNamespaceCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const ExampleWithNamespace operator*() const;
    const ExampleWithNamespace* operator->() const;
    const ExampleWithNamespaceCollectionIterator& operator++() const;

  private:
    mutable int m_index;
    mutable ExampleWithNamespace m_object;
    const ExampleWithNamespaceObjPointerContainer* m_collection;
};

/**
A Collection is identified by an ID.
*/

class ExampleWithNamespaceCollection : public podio::CollectionBase {

public:
  typedef const ExampleWithNamespaceCollectionIterator const_iterator;

  ExampleWithNamespaceCollection();
//  ExampleWithNamespaceCollection(const ExampleWithNamespaceCollection& ) = delete; // deletion doesn't work w/ ROOT IO ! :-(
//  ExampleWithNamespaceCollection(ExampleWithNamespaceVector* data, int collectionID);
  ~ExampleWithNamespaceCollection();

  void clear();
  /// Append a new object to the collection, and return this object.
  ExampleWithNamespace create();

  /// Append a new object to the collection, and return this object.
  /// Initialized with the parameters given
  template<typename... Args>
  ExampleWithNamespace create(Args&&... args);
  int size() const;

  /// Returns the object of given index
  const ExampleWithNamespace operator[](unsigned int index) const;
  /// Returns the object of given index
  const ExampleWithNamespace at(unsigned int index) const;


  /// Append object to the collection
  void push_back(ConstExampleWithNamespace object);

  void prepareForWrite();
  void prepareAfterRead();
  void setBuffer(void* address);
  bool setReferences(const podio::ICollectionProvider* collectionProvider);

  podio::CollRefCollection* referenceCollections() { return &m_refCollections;};

  void setID(unsigned ID){
    m_collectionID = ID;
    std::for_each(m_entries.begin(),m_entries.end(),
                 [ID](ExampleWithNamespaceObj* obj){obj->id = {obj->id.index,static_cast<int>(ID)}; }
    );
  };

  bool isValid() const {
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
  void* getBufferAddress() { return (void*)&m_data;};

  /// returns the pointer to the data buffer
  std::vector<ExampleWithNamespaceData>* _getBuffer() { return m_data;};

    template<size_t arraysize>
  const std::array<ex2::NamespaceStruct,arraysize> data() const;


private:
  bool m_isValid;
  int m_collectionID;
  ExampleWithNamespaceObjPointerContainer m_entries;
  // members to handle 1-to-N-relations

  // members to handle streaming
  podio::CollRefCollection m_refCollections;
  ExampleWithNamespaceDataContainer* m_data;
};

template<typename... Args>
ExampleWithNamespace  ExampleWithNamespaceCollection::create(Args&&... args){
  int size = m_entries.size();
  auto obj = new ExampleWithNamespaceObj({size,m_collectionID},{args...});
  m_entries.push_back(obj);
  return ExampleWithNamespace(obj);
}

template<size_t arraysize>
const std::array<class ex2::NamespaceStruct,arraysize> ExampleWithNamespaceCollection::data() const {
  std::array<class ex2::NamespaceStruct,arraysize> tmp;
  auto valid_size = std::min(arraysize,m_entries.size());
  for (unsigned i = 0; i<valid_size; ++i){
    tmp[i] = m_entries[i]->data.data;
 }
 return tmp;
}

} // namespace ex
#endif
