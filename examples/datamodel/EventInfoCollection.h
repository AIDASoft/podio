//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef EventInfoCollection_H
#define  EventInfoCollection_H

#include <string>
#include <vector>
#include <deque>
#include <array>

// podio specific includes
#include "podio/ICollectionProvider.h"
#include "podio/CollectionBase.h"
#include "podio/CollectionIDTable.h"

// datamodel specific includes
#include "EventInfoData.h"
#include "EventInfo.h"
#include "EventInfoObj.h"


typedef std::vector<EventInfoData> EventInfoDataContainer;
typedef std::deque<EventInfoObj*> EventInfoObjPointerContainer;

class EventInfoCollectionIterator {

  public:
    EventInfoCollectionIterator(int index, const EventInfoObjPointerContainer* collection) : m_index(index), m_object(nullptr), m_collection(collection) {}

    bool operator!=(const EventInfoCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const EventInfo operator*() const;
    const EventInfo* operator->() const;
    const EventInfoCollectionIterator& operator++() const;

  private:
    mutable int m_index;
    mutable EventInfo m_object;
    const EventInfoObjPointerContainer* m_collection;
};

/**
A Collection is identified by an ID.
*/

class EventInfoCollection : public podio::CollectionBase {

public:
  typedef const EventInfoCollectionIterator const_iterator;

  EventInfoCollection();
//  EventInfoCollection(const EventInfoCollection& ) = delete; // deletion doesn't work w/ ROOT IO ! :-(
//  EventInfoCollection(EventInfoVector* data, int collectionID);
  ~EventInfoCollection(){};

  void clear();
  /// Append a new object to the collection, and return this object.
  EventInfo create();

  /// Append a new object to the collection, and return this object.
  /// Initialized with the parameters given
  template<typename... Args>
  EventInfo create(Args&&... args);
  int size() const;

  /// Returns the object of given index
  const EventInfo operator[](unsigned int index) const;
  /// Returns the object of given index
  const EventInfo at(unsigned int index) const;


  /// Append object to the collection
  void push_back(ConstEventInfo object);

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
  std::vector<EventInfoData>* _getBuffer() { return m_data;};

     template<size_t arraysize>  
  const std::array<int,arraysize> Number() const;


private:
  int m_collectionID;
  EventInfoObjPointerContainer m_entries;
  // members to handle 1-to-N-relations

  // members to handle streaming
  podio::CollRefCollection* m_refCollections;
  EventInfoDataContainer* m_data;
};

template<typename... Args>
EventInfo  EventInfoCollection::create(Args&&... args){
  int size = m_entries.size();
  auto obj = new EventInfoObj({size,m_collectionID},{args...});
  m_entries.push_back(obj);
  return EventInfo(obj);
}

template<size_t arraysize>
const std::array<int,arraysize> EventInfoCollection::Number() const {
  std::array<int,arraysize> tmp;
  auto valid_size = std::min(arraysize,m_entries.size());
  for (unsigned i = 0; i<valid_size; ++i){
    tmp[i] = m_entries[i]->data.Number;
 }
 return tmp;
}


#endif
