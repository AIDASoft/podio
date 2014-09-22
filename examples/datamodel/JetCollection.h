//AUTOMATICALLY GENERATED - DO NOT EDIT

#ifndef JetCollection_H
#define  JetCollection_H

#include <string>
#include <vector>

// albers specific includes
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "Jet.h"
#include "JetHandle.h"

typedef std::vector<Jet> JetVector;
typedef std::vector<JetHandle> JetHandleVector;

class JetCollectionIterator {

  public:
    JetCollectionIterator(int index, const JetCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const JetCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }

    const JetHandle operator*() const;

    const JetCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const JetCollection* m_collection;
};

/**
A Collection is identified by an ID.
*/

class JetCollection : public albers::CollectionBase {

public:
  typedef const JetCollectionIterator const_iterator;

  JetCollection();
//  JetCollection(JetVector* data, int collectionID);
  ~JetCollection(){};

  void clear();
  /// Append a new object to the collection, and return a Handle to this object.
  JetHandle& create();

  /// Returns a Handle to the object at position index in the collection
  const JetHandle& get(int index) const;

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
  std::vector<Jet>* _getBuffer(){ return m_data;};
private:
  unsigned m_collectionID;
  JetVector* m_data;
  JetHandleVector m_handles;

};

#endif