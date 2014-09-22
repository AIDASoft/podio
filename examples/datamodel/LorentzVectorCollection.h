//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef LorentzVectorCollection_H
#define  LorentzVectorCollection_H

#include <string>
#include <vector>

// albers specific includes
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "LorentzVector.h"
#include "LorentzVectorHandle.h"

typedef std::vector<LorentzVector> LorentzVectorVector;
typedef std::vector<LorentzVectorHandle> LorentzVectorHandleVector;

class LorentzVectorCollectionIterator {

  public:
    LorentzVectorCollectionIterator(int index, const LorentzVectorCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const LorentzVectorCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const LorentzVectorHandle operator*() const;

    const LorentzVectorCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const LorentzVectorCollection* m_collection;
};

/**
A Collection is identified by an ID.
*/

class LorentzVectorCollection : public albers::CollectionBase {

public:
  typedef const LorentzVectorCollectionIterator const_iterator;

  LorentzVectorCollection();
//  LorentzVectorCollection(LorentzVectorVector* data, int collectionID);
  ~LorentzVectorCollection(){};

  void clear();
  /// Append a new object to the collection, and return a Handle to this object.
  LorentzVectorHandle& create();

  /// Returns a Handle to the object at position index in the collection
  const LorentzVectorHandle& get(int index) const;

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

  void* _getRawBuffer(){ return (void*)&m_data;};
  std::vector<LorentzVector>* _getBuffer(){ return m_data;};
private:
  unsigned m_collectionID;
  LorentzVectorVector* m_data;
  LorentzVectorHandleVector m_handles;

};

#endif