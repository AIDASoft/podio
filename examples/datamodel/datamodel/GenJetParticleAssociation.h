#ifndef GenJetParticleAssociation_H 
#define GenJetParticleAssociation_H

// Reference to the mc particle
// author: C. Bernet, B. Hegner

#include "datamodel/GenJetHandle.h"
#include "datamodel/MCParticleHandle.h"


class GenJetParticleAssociation {
public:
  GenJetHandle Jet; //Reference to the gen jet 
  MCParticleHandle Particle; //Reference to the mc particle 

};

#endif