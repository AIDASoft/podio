//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef SimCaloClusterCollection_H
#define  SimCaloClusterCollection_H

#include <string>
#include <vector>
#include <deque>

// albers specific includes
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "datamodel/SimCaloCluster.h"
#include "datamodel/SimCaloClusterHandle.h"

typedef std::vector<SimCaloCluster> SimCaloClusterVector;
typedef std::vector<SimCaloClusterHandle> SimCaloClusterHandleContainer;

class SimCaloClusterCollectionIterator {

  public:
    SimCaloClusterCollectionIterator(int index, const SimCaloClusterCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const SimCaloClusterCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const SimCaloClusterHandle operator*() const;

    const SimCaloClusterCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const SimCaloClusterCollection* m_collection;
};

/**
A Collection is identified by an ID.
*/

class SimCaloClusterCollection : public albers::CollectionBase {

public:
  typedef const SimCaloClusterCollectionIterator const_iterator;

  SimCaloClusterCollection();
//  SimCaloClusterCollection(SimCaloClusterVector* data, int collectionID);
  ~SimCaloClusterCollection(){};

  void clear();

  /// Append a new object to the collection, and return a Handle to this object.
  SimCaloClusterHandle& create();

  /// Insert an existing handle into the collection. 
  /// In this operation, the data pointed by the handle is copied.
  SimCaloClusterHandle& insert(const SimCaloClusterHandle& origin);  
  
  /// Returns a Handle to the object at position index in the collection
  const SimCaloClusterHandle& get(int index) const;

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
  std::vector<SimCaloCluster>* _getBuffer() { return m_data;};

  /// returns the collection of Handles
  const SimCaloClusterHandleContainer& getHandles() { return m_handles; }

  /// print some information
  void print() const;


private:
  unsigned m_collectionID;
  SimCaloClusterVector* m_data;
  SimCaloClusterHandleContainer m_handles;
  // members to handle 1-to-N-relations
  
};

#endif