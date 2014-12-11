//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef CaloHitCollection_H
#define  CaloHitCollection_H

#include <string>
#include <vector>
#include <deque>

// albers specific includes
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "datamodel/CaloHit.h"
#include "datamodel/CaloHitHandle.h"

typedef std::vector<CaloHit> CaloHitVector;
typedef std::vector<CaloHitHandle> CaloHitHandleContainer;

class CaloHitCollectionIterator {

  public:
    CaloHitCollectionIterator(int index, const CaloHitCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const CaloHitCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const CaloHitHandle operator*() const;

    const CaloHitCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const CaloHitCollection* m_collection;
};

/**
A Collection is identified by an ID.
*/

class CaloHitCollection : public albers::CollectionBase {

public:
  typedef const CaloHitCollectionIterator const_iterator;

  CaloHitCollection();
//  CaloHitCollection(CaloHitVector* data, int collectionID);
  ~CaloHitCollection(){};

  void clear();

  /// Append a new object to the collection, and return a Handle to this object.
  CaloHitHandle& create();

  /// Insert an existing handle into the collection. 
  /// In this operation, the data pointed by the handle is copied.
  CaloHitHandle& insert(const CaloHitHandle& origin);  
  
  /// Returns a Handle to the object at position index in the collection
  const CaloHitHandle& get(int index) const;

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
  std::vector<CaloHit>* _getBuffer() { return m_data;};

  /// returns the collection of Handles
  const CaloHitHandleContainer& getHandles() { return m_handles; }

  /// print some information
  void print() const;


private:
  unsigned m_collectionID;
  CaloHitVector* m_data;
  CaloHitHandleContainer m_handles;
  // members to handle 1-to-N-relations
  
};

#endif