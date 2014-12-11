//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef TrackStateCollection_H
#define  TrackStateCollection_H

#include <string>
#include <vector>
#include <deque>

// albers specific includes
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "datamodel/TrackState.h"
#include "datamodel/TrackStateHandle.h"

typedef std::vector<TrackState> TrackStateVector;
typedef std::vector<TrackStateHandle> TrackStateHandleContainer;

class TrackStateCollectionIterator {

  public:
    TrackStateCollectionIterator(int index, const TrackStateCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const TrackStateCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const TrackStateHandle operator*() const;

    const TrackStateCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const TrackStateCollection* m_collection;
};

/**
A Collection is identified by an ID.
*/

class TrackStateCollection : public albers::CollectionBase {

public:
  typedef const TrackStateCollectionIterator const_iterator;

  TrackStateCollection();
//  TrackStateCollection(TrackStateVector* data, int collectionID);
  ~TrackStateCollection(){};

  void clear();

  /// Append a new object to the collection, and return a Handle to this object.
  TrackStateHandle& create();

  /// Insert an existing handle into the collection. 
  /// In this operation, the data pointed by the handle is copied.
  TrackStateHandle& insert(const TrackStateHandle& origin);  
  
  /// Returns a Handle to the object at position index in the collection
  const TrackStateHandle& get(int index) const;

  /// Currently does nothing
  void prepareForWrite(const albers::Registry* registry);
  void prepareAfterRead(albers::Registry* registry);
  void setPODsAddress(const void* address);

  void setID(unsigned ID){m_collectionID = ID;};

  // support for the iterator protocol
  const const_iterator begin() const {
    return const_iterator(0, this);
  }
  const	const_iterator end() const {
    return const_iterator(m_handles.size(), this);
  }

//  std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections();

  /// returns the address of the pointer to the data buffer
  void* _getRawBuffer() { return (void*)&m_data;};

  /// returns the pointer to the data buffer
  std::vector<TrackState>* _getBuffer() { return m_data;};

  /// returns the collection of Handles
  const TrackStateHandleContainer& getHandles() { return m_handles; }

  /// print some information
  void print() const;


private:
  unsigned m_collectionID;
  TrackStateVector* m_data;
  TrackStateHandleContainer m_handles;
  // members to handle 1-to-N-relations
  
};

#endif