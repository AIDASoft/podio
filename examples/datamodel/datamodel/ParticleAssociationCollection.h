//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef ParticleAssociationCollection_H
#define  ParticleAssociationCollection_H

#include <string>
#include <vector>
#include <deque>

// albers specific includes
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "datamodel/ParticleAssociation.h"
#include "datamodel/ParticleAssociationHandle.h"

typedef std::vector<ParticleAssociation> ParticleAssociationVector;
typedef std::deque<ParticleAssociationHandle> ParticleAssociationHandleContainer;

class ParticleAssociationCollectionIterator {

  public:
    ParticleAssociationCollectionIterator(int index, const ParticleAssociationCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const ParticleAssociationCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const ParticleAssociationHandle operator*() const;

    const ParticleAssociationCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const ParticleAssociationCollection* m_collection;
};

/**
A Collection is identified by an ID.
*/

class ParticleAssociationCollection : public albers::CollectionBase {

public:
  typedef const ParticleAssociationCollectionIterator const_iterator;

  ParticleAssociationCollection();
//  ParticleAssociationCollection(ParticleAssociationVector* data, int collectionID);
  ~ParticleAssociationCollection(){};

  void clear();
  /// Append a new object to the collection, and return a Handle to this object.
  ParticleAssociationHandle& create();

  /// Returns a Handle to the object at position index in the collection
  const ParticleAssociationHandle& get(int index) const;

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
  std::vector<ParticleAssociation>* _getBuffer() { return m_data;};

  /// returns the collection of Handles
  const ParticleAssociationHandleContainer& getHandles() { return m_handles; }

  /// print some information
  void print() const;


private:
  unsigned m_collectionID;
  ParticleAssociationVector* m_data;
  ParticleAssociationHandleContainer m_handles;
  // members to handle 1-to-N-relations
  
};

#endif