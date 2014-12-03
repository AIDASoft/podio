#ifndef ParticleClusterAssociation_H 
#define ParticleClusterAssociation_H

// The cluster
// author: C. Bernet, B. Hegner

#include "datamodel/ParticleHandle.h"
#include "datamodel/CaloClusterHandle.h"


class ParticleClusterAssociation {
public:
  ParticleHandle Particle; //The particle 
  CaloClusterHandle Cluster; //The cluster 

};

#endif