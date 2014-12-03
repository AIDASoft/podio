#include "JetUtils.h"

#include "ParticleHandle.h"
#include "JetHandle.h"
#include "JetParticleAssociationCollection.h"
#include <iostream>

namespace utils {

  std::vector<ParticleHandle> associatedParticles( const JetHandle& jet,
						   const JetParticleAssociationCollection& associations ) {

    std::vector<ParticleHandle> result;
    if(not jet.isAvailable()) {
      std::cerr<<"Jet handle is not available"<<std::endl;
      return result;
      // COLIN should throw an exception
    }
    for(const auto& assoch : associations){
      const JetParticleAssociation& assoc = assoch.read();
      if( assoc.Jet == jet ) {
	result.emplace_back(assoc.Particle);
      }
    }
    return result;
  }

} // namespace
