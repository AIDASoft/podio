//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef TrackClusterCollection_H
#define  TrackClusterCollection_H

#include <string>
#include <vector>
#include <deque>

// albers specific includes
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "datamodel/TrackCluster.h"
#include "datamodel/TrackClusterHandle.h"

typedef std::vector<TrackCluster> TrackClusterVector;
typedef std::vector<TrackClusterHandle> TrackClusterHandleContainer;

class TrackClusterCollectionIterator {

  public:
    TrackClusterCollectionIterator(int index, const TrackClusterCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const TrackClusterCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const TrackClusterHandle operator*() const;

    const TrackClusterCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const TrackClusterCollection* m_collection;
};

/**
A Collection is identified by an ID.
*/

class TrackClusterCollection : public albers::CollectionBase {

public:
  typedef const TrackClusterCollectionIterator const_iterator;

  TrackClusterCollection();
//  TrackClusterCollection(TrackClusterVector* data, int collectionID);
  ~TrackClusterCollection(){};

  void clear();

  /// Append a new object to the collection, and return a Handle to this object.
  TrackClusterHandle& create();

  /// Insert an existing handle into the collection. 
  /// In this operation, the data pointed by the handle is copied.
  TrackClusterHandle& insert(const TrackClusterHandle& origin);  
  
  /// Returns a Handle to the object at position index in the collection
  const TrackClusterHandle& get(int index) const;

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
  std::vector<TrackCluster>* _getBuffer() { return m_data;};

  /// returns the collection of Handles
  const TrackClusterHandleContainer& getHandles() { return m_handles; }

  /// print some information
  void print() const;


private:
  unsigned m_collectionID;
  TrackClusterVector* m_data;
  TrackClusterHandleContainer m_handles;
  // members to handle 1-to-N-relations
  
};

#endif