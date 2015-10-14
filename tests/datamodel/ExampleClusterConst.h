#ifndef ConstExampleCluster_H
#define ConstExampleCluster_H
#include "ExampleClusterData.h"
#include <vector>
#include "ExampleHit.h"

#include <vector>
#include "podio/ObjectID.h"

//  cluster energy
// author: B. Hegner

//forward declarations
class ExampleClusterCollection;
class ExampleClusterCollectionIterator;


#include "ExampleClusterObj.h"

class ConstExampleCluster {

  friend ExampleCluster;
  friend ExampleClusterCollection;
  friend ExampleClusterCollectionIterator;

public:

  /// default constructor
  ConstExampleCluster();
  ConstExampleCluster(double energy);

  /// constructor from existing ExampleClusterObj
  ConstExampleCluster(ExampleClusterObj* obj);
  /// copy constructor
  ConstExampleCluster(const ConstExampleCluster& other);
  /// copy-assignment operator
  ConstExampleCluster& operator=(const ConstExampleCluster& other);
  /// support cloning (deep-copy)
  ConstExampleCluster clone() const;
  /// destructor
  ~ConstExampleCluster();


public:

  const double& energy() const { return m_obj->data.energy; };

  unsigned int Hits_size() const;
  ConstExampleHit Hits(unsigned int) const;
  std::vector<ConstExampleHit>::const_iterator Hits_begin() const;
  std::vector<ConstExampleHit>::const_iterator Hits_end() const;

  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleClusterObj instance
  void unlink(){m_obj = nullptr;};

  bool operator==(const ConstExampleCluster& other) const {
       return (m_obj==other.m_obj);
  }

  bool operator==(const ExampleCluster& other) const;

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleCluster& p1,
//       const ExampleCluster& p2 );

  const podio::ObjectID getObjectID() const;

private:
  ExampleClusterObj* m_obj;

};

#endif
