#ifndef ReferencingDataCollection_H
#define  ReferencingDataCollection_H

#include <string>
#include <vector>

// albers specific includes 
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "ReferencingData.h"
#include "ReferencingDataHandle.h"

typedef std::vector<ReferencingData> ReferencingDataVector;
typedef std::vector<ReferencingDataHandle> ReferencingDataHandleVector;

class ReferencingDataCollectionIterator {

  public:
    ReferencingDataCollectionIterator(int index, const ReferencingDataCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const ReferencingDataCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }
         
    const ReferencingDataHandle operator*() const;
         
    const ReferencingDataCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const ReferencingDataCollection* m_collection;
};

class ReferencingDataCollection : public albers::CollectionBase {

public:
  typedef const ReferencingDataCollectionIterator const_iterator;

  ReferencingDataCollection();
//  ReferencingDataCollection(ReferencingDataVector* data, int collectionID);
  ~ReferencingDataCollection(){};

  void clear();
  ReferencingDataHandle create();
  ReferencingDataHandle get(int index) const;

  void prepareForWrite(const albers::Registry* registry);
  void prepareAfterRead(albers::Registry* registry);
  void setPODsAddress(const void* address);
  void setID(int ID){m_collectionID = ID;};

  // support for the iterator protocol
  const const_iterator begin() const {
    return const_iterator(0, this);
  }
  const	const_iterator end() const { 
    return const_iterator(m_handles.size(), this);
  }

  void* _getRawBuffer(){ return (void*)&m_data;};
  std::vector<ReferencingData>* _getBuffer(){ return m_data;};
private:
  int m_collectionID;
  ReferencingDataVector* m_data;
  ReferencingDataHandleVector m_handles;

};

#endif