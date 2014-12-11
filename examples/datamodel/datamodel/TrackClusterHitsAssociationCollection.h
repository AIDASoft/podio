//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef TrackClusterHitsAssociationCollection_H
#define  TrackClusterHitsAssociationCollection_H

#include <string>
#include <vector>
#include <deque>

// albers specific includes
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "datamodel/TrackClusterHitsAssociation.h"
#include "datamodel/TrackClusterHitsAssociationHandle.h"

typedef std::vector<TrackClusterHitsAssociation> TrackClusterHitsAssociationVector;
typedef std::vector<TrackClusterHitsAssociationHandle> TrackClusterHitsAssociationHandleContainer;

class TrackClusterHitsAssociationCollectionIterator {

  public:
    TrackClusterHitsAssociationCollectionIterator(int index, const TrackClusterHitsAssociationCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const TrackClusterHitsAssociationCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const TrackClusterHitsAssociationHandle operator*() const;

    const TrackClusterHitsAssociationCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const TrackClusterHitsAssociationCollection* m_collection;
};

/**
A Collection is identified by an ID.
*/

class TrackClusterHitsAssociationCollection : public albers::CollectionBase {

public:
  typedef const TrackClusterHitsAssociationCollectionIterator const_iterator;

  TrackClusterHitsAssociationCollection();
//  TrackClusterHitsAssociationCollection(TrackClusterHitsAssociationVector* data, int collectionID);
  ~TrackClusterHitsAssociationCollection(){};

  void clear();

  /// Append a new object to the collection, and return a Handle to this object.
  TrackClusterHitsAssociationHandle& create();

  /// Insert an existing handle into the collection. 
  /// In this operation, the data pointed by the handle is copied.
  TrackClusterHitsAssociationHandle& insert(const TrackClusterHitsAssociationHandle& origin);  
  
  /// Returns a Handle to the object at position index in the collection
  const TrackClusterHitsAssociationHandle& get(int index) const;

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
  std::vector<TrackClusterHitsAssociation>* _getBuffer() { return m_data;};

  /// returns the collection of Handles
  const TrackClusterHitsAssociationHandleContainer& getHandles() { return m_handles; }

  /// print some information
  void print() const;


private:
  unsigned m_collectionID;
  TrackClusterHitsAssociationVector* m_data;
  TrackClusterHitsAssociationHandleContainer m_handles;
  // members to handle 1-to-N-relations
  
};

#endif