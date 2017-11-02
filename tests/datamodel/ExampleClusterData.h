#ifndef ExampleClusterDATA_H
#define ExampleClusterDATA_H

/** @class ExampleClusterData
 *  Cluster
 *  @author: B. Hegner
 */

class ExampleClusterData {
public:
  double energy; ///< cluster energy
  unsigned int Hits_begin;
  unsigned int Hits_end;
  unsigned int Clusters_begin;
  unsigned int Clusters_end;
};

#endif
