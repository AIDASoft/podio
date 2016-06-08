#ifndef ExampleCluster_H
#define ExampleCluster_H
#include "ExampleClusterData.h"
#include <vector>
#include "ExampleHit.h"
#include "ExampleCluster.h"
#include <vector>
#include "podio/ObjectID.h"

// Cluster
// author: B. Hegner

//forward declarations


#include "ExampleClusterObj.h"



class ExampleClusterCollection;
class ExampleClusterCollectionIterator;

class ExampleCluster {

  friend ExampleClusterCollection;
  friend ExampleClusterCollectionIterator;

public:

  /// default constructor
  ExampleCluster();
  ExampleCluster(double energy);

  /// constructor from existing ExampleClusterObj
  ExampleCluster(ExampleClusterObj* obj);
  /// copy constructor
  ExampleCluster(const ExampleCluster& other);
  /// copy-assignment operator
  ExampleCluster& operator=(const ExampleCluster& other);
  /// support cloning (deep-copy)
  ExampleCluster clone() const;
  /// destructor
  ~ExampleCluster();

public:

  const double& energy() const;

  void energy(double value);


  void addHits(ExampleHit);
  unsigned int Hits_size() const;
  ExampleHit Hits(unsigned int) const;
  std::vector<ExampleHit>::const_iterator Hits_begin() const;
  std::vector<ExampleHit>::const_iterator Hits_end() const;

  void addClusters(ExampleCluster);
  unsigned int Clusters_size() const;
  ExampleCluster Clusters(unsigned int) const;
  std::vector<ExampleCluster>::const_iterator Clusters_begin() const;
  std::vector<ExampleCluster>::const_iterator Clusters_end() const;



  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleClusterObj instance
  void unlink(){m_obj = nullptr;}

  bool operator==(const ExampleCluster& other) const {
    return (m_obj==other.m_obj);
  }

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleCluster& p1,
//       const ExampleCluster& p2 );
  bool operator<(const ExampleCluster& other) const { return m_obj < other.m_obj  ; }

  const podio::ObjectID getObjectID() const;

private:
  ExampleClusterObj* m_obj;

};



#endif
