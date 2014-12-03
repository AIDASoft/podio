#ifndef SimCaloClusterHitAssociation_H 
#define SimCaloClusterHitAssociation_H

// The hit
// author: C. Bernet, B. Hegner

#include "datamodel/SimCaloClusterHandle.h"
#include "datamodel/SimCaloHitHandle.h"


class SimCaloClusterHitAssociation {
public:
  SimCaloClusterHandle Cluster; //The cluster 
  SimCaloHitHandle Hit; //The hit 

};

#endif