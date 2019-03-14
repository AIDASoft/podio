#ifndef ExampleCluster_H
#define ExampleCluster_H
#include "ExampleCluster.h"
#include "ExampleClusterData.h"
#include "ExampleHit.h"
#include "podio/ObjectID.h"
#include <iomanip>
#include <iostream>
#include <vector>

// forward declarations

#include "ExampleClusterConst.h"
#include "ExampleClusterObj.h"

class ExampleClusterCollection;
class ExampleClusterCollectionIterator;
class ConstExampleCluster;

/** @class ExampleCluster
 *  Cluster
 *  @author: B. Hegner
 */
class ExampleCluster {

  friend ExampleClusterCollection;
  friend ExampleClusterCollectionIterator;
  friend ConstExampleCluster;

public:
  /// default constructor
  ExampleCluster();
  ExampleCluster(double energy);

  /// constructor from existing ExampleClusterObj
  ExampleCluster(ExampleClusterObj *obj);
  /// copy constructor
  ExampleCluster(const ExampleCluster &other);
  /// copy-assignment operator
  ExampleCluster &operator=(const ExampleCluster &other);
  /// support cloning (deep-copy)
  ExampleCluster clone() const;
  /// destructor
  ~ExampleCluster();

  /// conversion to const object
  operator ConstExampleCluster() const;

public:
  /// Access the  cluster energy
  const double &energy() const;

  /// Set the  cluster energy
  void energy(double value);

  void addHits(::ConstExampleHit);
  unsigned int Hits_size() const;
  ::ConstExampleHit Hits(unsigned int) const;
  std::vector<::ConstExampleHit>::const_iterator Hits_begin() const;
  std::vector<::ConstExampleHit>::const_iterator Hits_end() const;

  void addClusters(::ConstExampleCluster);
  unsigned int Clusters_size() const;
  ::ConstExampleCluster Clusters(unsigned int) const;
  std::vector<::ConstExampleCluster>::const_iterator Clusters_begin() const;
  std::vector<::ConstExampleCluster>::const_iterator Clusters_end() const;

  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleClusterObj instance
  void unlink() { m_obj = nullptr; }

  bool operator==(const ExampleCluster &other) const {
    return (m_obj == other.m_obj);
  }

  bool operator==(const ConstExampleCluster &other) const;

  // less comparison operator, so that objects can be e.g. stored in sets.
  //  friend bool operator< (const ExampleCluster& p1,
  //       const ExampleCluster& p2 );
  bool operator<(const ExampleCluster &other) const {
    return m_obj < other.m_obj;
  }

  unsigned int id() const {
    return getObjectID().collectionID * 10000000 + getObjectID().index;
  }

  const podio::ObjectID getObjectID() const;

private:
  ExampleClusterObj *m_obj;
};

std::ostream &operator<<(std::ostream &o, const ConstExampleCluster &value);

#endif
