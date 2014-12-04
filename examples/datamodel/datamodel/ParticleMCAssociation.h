#ifndef ParticleMCAssociation_H 
#define ParticleMCAssociation_H

// Reference to the Monte-Carlo particle
// author: C. Bernet, B. Hegner

#include "datamodel/ParticleHandle.h"
#include "datamodel/MCParticleHandle.h"


class ParticleMCAssociation {
public:
  ParticleHandle Rec; //Reference to the reconstructed particle 
  MCParticleHandle Sim; //Reference to the Monte-Carlo particle 

};

#endif