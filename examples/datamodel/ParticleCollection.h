#ifndef ParticleCollection_H
#define  ParticleCollection_H

#include <string>
#include <vector>

// albers specific includes 
#include "albers/Registry.h"
#include "albers/CollectionBase.h"

// datamodel specific includes
#include "Particle.h"
#include "ParticleHandle.h"

typedef std::vector<Particle> ParticleVector;
typedef std::vector<ParticleHandle> ParticleHandleVector;

class ParticleCollectionIterator {

  public:
    ParticleCollectionIterator(int index, const ParticleCollection* collection) : m_index(index), m_collection(collection) {}

    bool operator!=(const ParticleCollectionIterator& x) const {
      return m_index != x.m_index; //TODO: may not be complete
    }
         
    const ParticleHandle operator*() const;
         
    const ParticleCollectionIterator& operator++() const {
      ++m_index;
      return *this;
    }

  private:
    mutable int m_index;
    const ParticleCollection* m_collection;
};

class ParticleCollection : public albers::CollectionBase {

public:
  typedef const ParticleCollectionIterator const_iterator;

  ParticleCollection();
//  ParticleCollection(ParticleVector* data, int collectionID);
  ~ParticleCollection(){};

  void clear();
  ParticleHandle create();
  ParticleHandle get(int index) const;

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
  std::vector<Particle>* _getBuffer(){ return m_data;};
private:
  int m_collectionID;
  ParticleVector* m_data;
  ParticleHandleVector m_handles;

};

#endif