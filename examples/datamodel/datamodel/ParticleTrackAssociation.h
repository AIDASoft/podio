#ifndef ParticleTrackAssociation_H 
#define ParticleTrackAssociation_H

// The track
// author: C. Bernet, B. Hegner

#include "datamodel/ParticleHandle.h"
#include "datamodel/TrackHandle.h"


class ParticleTrackAssociation {
public:
  ParticleHandle Particle; //The particle 
  TrackHandle Track; //The track 

};

#endif