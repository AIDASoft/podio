//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef MissingEnergyCollection_H
#define  MissingEnergyCollection_H

#include <string>
#include <vector>

// albers specific includes
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "MissingEnergy.h"
#include "MissingEnergyHandle.h"

typedef std::vector<MissingEnergy> MissingEnergyVector;
typedef std::vector<MissingEnergyHandle> MissingEnergyHandleVector;

class MissingEnergyCollectionIterator {

  public:
    MissingEnergyCollectionIterator(int index, const MissingEnergyCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const MissingEnergyCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const MissingEnergyHandle operator*() const;

    const MissingEnergyCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const MissingEnergyCollection* m_collection;
};

/**
A Collection is identified by an ID.
*/

class MissingEnergyCollection : public albers::CollectionBase {

public:
  typedef const MissingEnergyCollectionIterator const_iterator;

  MissingEnergyCollection();
//  MissingEnergyCollection(MissingEnergyVector* data, int collectionID);
  ~MissingEnergyCollection(){};

  void clear();
  /// Append a new object to the collection, and return a Handle to this object.
  MissingEnergyHandle& create();

  /// Returns a Handle to the object at position index in the collection
  const MissingEnergyHandle& get(int index) const;

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
  std::vector<MissingEnergy>* _getBuffer(){ return m_data;};
private:
  unsigned m_collectionID;
  MissingEnergyVector* m_data;
  MissingEnergyHandleVector m_handles;

};

#endif