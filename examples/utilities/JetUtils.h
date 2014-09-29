#ifndef UTILS_JET_H
#define UTILS_JET_H

#include <vector>

class ParticleHandle;
class JetHandle;
class JetParticleAssociationCollection;

namespace utils {

  std::vector<ParticleHandle> associatedParticles( const JetHandle& jet,
						   const JetParticleAssociationCollection& associations );
}

#endif
