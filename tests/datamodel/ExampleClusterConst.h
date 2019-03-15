#ifndef ConstExampleCluster_H
#define ConstExampleCluster_H
#include "ExampleCluster.h"
#include "ExampleClusterData.h"
#include "ExampleHit.h"
#include "podio/ObjectID.h"
#include <vector>

// forward declarations

#include "ExampleClusterObj.h"

class ExampleClusterObj;
class ExampleCluster;
class ExampleClusterCollection;
class ExampleClusterCollectionIterator;

/** @class ConstExampleCluster
 *  Cluster
 *  @author: B. Hegner
 */

class ConstExampleCluster {

  friend ExampleCluster;
  friend ExampleClusterCollection;
  friend ExampleClusterCollectionIterator;

public:
  /// default constructor
  ConstExampleCluster();
  ConstExampleCluster(double energy);

  /// constructor from existing ExampleClusterObj
  ConstExampleCluster(ExampleClusterObj *obj);
  /// copy constructor
  ConstExampleCluster(const ConstExampleCluster &other);
  /// copy-assignment operator
  ConstExampleCluster &operator=(const ConstExampleCluster &other);
  /// support cloning (deep-copy)
  ConstExampleCluster clone() const;
  /// destructor
  ~ConstExampleCluster();

public:
  /// Access the  cluster energy
  const double &energy() const;

  unsigned int Hits_size() const;
  ::ConstExampleHit Hits(unsigned int) const;
  std::vector<::ConstExampleHit>::const_iterator Hits_begin() const;
  std::vector<::ConstExampleHit>::const_iterator Hits_end() const;
  unsigned int Clusters_size() const;
  ::ConstExampleCluster Clusters(unsigned int) const;
  std::vector<::ConstExampleCluster>::const_iterator Clusters_begin() const;
  std::vector<::ConstExampleCluster>::const_iterator Clusters_end() const;

  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleClusterObj instance
  void unlink() { m_obj = nullptr; }

  bool operator==(const ConstExampleCluster &other) const {
    return (m_obj == other.m_obj);
  }

  bool operator==(const ExampleCluster &other) const;

  // less comparison operator, so that objects can be e.g. stored in sets.
  //  friend bool operator< (const ExampleCluster& p1,
  //       const ExampleCluster& p2 );
  bool operator<(const ConstExampleCluster &other) const {
    return m_obj < other.m_obj;
  }

  unsigned int id() const {
    return getObjectID().collectionID * 10000000 + getObjectID().index;
  }

  const podio::ObjectID getObjectID() const;

private:
  ExampleClusterObj *m_obj;
};

#endif
