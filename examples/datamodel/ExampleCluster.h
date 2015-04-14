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

//forward declarations
class ExampleClusterCollection;
class ExampleClusterCollectionIterator;
class ExampleClusterObj;

#include "ExampleClusterObj.h"

namespace albers {
  class Registry;
}


class ExampleCluster {

  friend ExampleClusterCollection;
  friend ExampleClusterCollectionIterator;

public:

  ExampleCluster();
  ExampleCluster(const ExampleCluster& other);
  ExampleCluster& operator=(const ExampleCluster& other);
  ExampleCluster(ExampleClusterObj* obj);
  ~ExampleCluster();

  const double& energy() const;

  void energy(double value);

  void addHits(ExampleHit&);
  std::vector<ExampleHit>::const_iterator Hits_begin() const;
  std::vector<ExampleHit>::const_iterator Hits_end() const;

  bool isAvailable() const; // precheck whether the pointee actually exists
  void unlink(){m_obj = nullptr;};

  bool operator==(const ExampleCluster& other) const {
       return (m_obj==other.m_obj);
  }

  /// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleCluster& p1,
//       const ExampleCluster& p2 );

  const albers::ObjectID getObjectID() const;

private:
  ExampleClusterObj* m_obj;

};

#endif
