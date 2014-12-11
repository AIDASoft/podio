//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef JetParticleAssociationCollection_H
#define  JetParticleAssociationCollection_H

#include <string>
#include <vector>
#include <deque>

// albers specific includes
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "datamodel/JetParticleAssociation.h"
#include "datamodel/JetParticleAssociationHandle.h"

typedef std::vector<JetParticleAssociation> JetParticleAssociationVector;
typedef std::vector<JetParticleAssociationHandle> JetParticleAssociationHandleContainer;

class JetParticleAssociationCollectionIterator {

  public:
    JetParticleAssociationCollectionIterator(int index, const JetParticleAssociationCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const JetParticleAssociationCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const JetParticleAssociationHandle operator*() const;

    const JetParticleAssociationCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const JetParticleAssociationCollection* m_collection;
};

/**
A Collection is identified by an ID.
*/

class JetParticleAssociationCollection : public albers::CollectionBase {

public:
  typedef const JetParticleAssociationCollectionIterator const_iterator;

  JetParticleAssociationCollection();
//  JetParticleAssociationCollection(JetParticleAssociationVector* data, int collectionID);
  ~JetParticleAssociationCollection(){};

  void clear();

  /// Append a new object to the collection, and return a Handle to this object.
  JetParticleAssociationHandle& create();

  /// Insert an existing handle into the collection. 
  /// In this operation, the data pointed by the handle is copied.
  JetParticleAssociationHandle& insert(const JetParticleAssociationHandle& origin);  
  
  /// Returns a Handle to the object at position index in the collection
  const JetParticleAssociationHandle& get(int index) const;

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
  std::vector<JetParticleAssociation>* _getBuffer() { return m_data;};

  /// returns the collection of Handles
  const JetParticleAssociationHandleContainer& getHandles() { return m_handles; }

  /// print some information
  void print() const;


private:
  unsigned m_collectionID;
  JetParticleAssociationVector* m_data;
  JetParticleAssociationHandleContainer m_handles;
  // members to handle 1-to-N-relations
  
};

#endif