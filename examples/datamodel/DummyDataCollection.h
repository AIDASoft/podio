//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef DummyDataCollection_H
#define  DummyDataCollection_H

#include <string>
#include <vector>

// albers specific includes 
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "DummyData.h"
#include "DummyDataHandle.h"

typedef std::vector<DummyData> DummyDataVector;
typedef std::vector<DummyDataHandle> DummyDataHandleVector;

class DummyDataCollectionIterator {

  public:
    DummyDataCollectionIterator(int index, const DummyDataCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const DummyDataCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }
         
    const DummyDataHandle operator*() const;
         
    const DummyDataCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const DummyDataCollection* m_collection;
};

/**
A Collection is identified by an ID.
*/

class DummyDataCollection : public albers::CollectionBase {

public:
  typedef const DummyDataCollectionIterator const_iterator;

  DummyDataCollection();
//  DummyDataCollection(DummyDataVector* data, int collectionID);
  ~DummyDataCollection(){};

  void clear();
  //COLIN: why not returning reference instead of a copy? 
  /// Append a new object to the collection, and return a Handle to this object. 
  DummyDataHandle create();

  /// Returns a Handle to the object at position index in the collection
  DummyDataHandle get(int index) const;

  /// Currently does nothing
  void prepareForWrite(const albers::Registry* registry);
  void prepareAfterRead(albers::Registry* registry);
  void setPODsAddress(const void* address);
  // COLIN: I don't see where setID is called, nor where m_collectionID is accessed outside this class. Tested that this function can be commented
  void setID(int ID){m_collectionID = ID;};

  // support for the iterator protocol
  const const_iterator begin() const {
    return const_iterator(0, this);
  }
  const	const_iterator end() const { 
    return const_iterator(m_handles.size(), this);
  }

  void* _getRawBuffer(){ return (void*)&m_data;};
  std::vector<DummyData>* _getBuffer(){ return m_data;};
private:
  // COLIN: uninitialized? in fact it seems the Registy is responsible for handling POD addresses
  int m_collectionID;
  DummyDataVector* m_data;
  DummyDataHandleVector m_handles;

};

#endif