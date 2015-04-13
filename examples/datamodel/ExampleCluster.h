#ifndef ExampleCluster_H
#define ExampleCluster_H
#include "ExampleClusterData.h"
#include <vector>
#include "ExampleHit.h"

#include <vector>
#include "albers/ObjectID.h"
#include <iostream>

// cluster energy
// author: B. Hegner

//forward declaration of ExampleCluster container
class ExampleClusterCollection;
class ExampleClusterCollectionIterator;

#include "ExampleClusterEntry.h"

namespace albers {
  class Registry;
}

class ExampleCluster {

  friend ExampleClusterCollection;
  friend ExampleClusterCollectionIterator;

public:

  ExampleCluster() : m_entry(new ExampleClusterEntry()){};
  ExampleCluster(const ExampleCluster& other) : m_entry(other.m_entry) {m_entry->increaseRefCount();};
  ExampleCluster& operator=(const ExampleCluster& other);
  ExampleCluster(ExampleClusterEntry* entry);
  ~ExampleCluster();

  const double& energy() const;

  void energy(double value);

  void addHits(ExampleHit&);
  std::vector<ExampleHit>::const_iterator Hits_begin() const;
  std::vector<ExampleHit>::const_iterator Hits_end() const;

  bool isAvailable() const; // precheck whether the pointee actually exists
  void unlink(){m_entry = nullptr;};

  bool operator==(const ExampleCluster& other) const {
       return (m_entry==other.m_entry);
  }

  /// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleCluster& p1,
//       const ExampleCluster& p2 );

  const albers::ObjectID getObjectID() const;

private:
  ExampleClusterEntry* m_entry;

};

#endif
