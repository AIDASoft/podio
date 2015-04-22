#ifndef ExampleCluster_H
#define ExampleCluster_H
#include "ExampleClusterData.h"
#include <vector>
#include "ExampleHit.h"

#include <vector>
#include "albers/ObjectID.h"

//  cluster energy
// author: B. Hegner

//forward declarations
class ExampleClusterCollection;
class ExampleClusterCollectionIterator;
class ExampleClusterObj;

#include "ExampleClusterObj.h"

class ExampleCluster {

  friend ExampleClusterCollection;
  friend ExampleClusterCollectionIterator;

public:

  /// default constructor
  ExampleCluster();
    ExampleCluster(double energy);

  /// copy constructor
  ExampleCluster(const ExampleCluster& other);
  /// copy-assignment operator
  ExampleCluster& operator=(const ExampleCluster& other);
  /// constructor from existing ExampleClusterObj
  ExampleCluster(ExampleClusterObj* obj);
  /// support cloning (deep-copy)
  ExampleCluster clone() const;
  /// destructor
  ~ExampleCluster();

  const double& energy() const { return m_obj->data.energy; };
  double& energy() { return m_obj->data.energy; };

  void energy(double value) { m_obj->data.energy = value; };

  void addHits(ExampleHit&);
  std::vector<ExampleHit>::const_iterator Hits_begin() const;
  std::vector<ExampleHit>::const_iterator Hits_end() const;

  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleClusterObj instance
  void unlink(){m_obj = nullptr;};

  bool operator==(const ExampleCluster& other) const {
       return (m_obj==other.m_obj);
  }

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleCluster& p1,
//       const ExampleCluster& p2 );

  const albers::ObjectID getObjectID() const;

private:
  ExampleClusterObj* m_obj;

};

#endif
