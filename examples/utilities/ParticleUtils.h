#ifndef UTILS_PARTICLE_H
#define UTILS_PARTICLE_H

#include <vector>
#include <iostream>

class ParticleHandle;
class LorentzVector;

namespace utils {

  /// returns the ParticleHandles that are in p1s but not in p2s.
  ///
  /// This algorithm preserves ordering and possible duplications in p1s.
  std::vector<ParticleHandle> unused(const std::vector<ParticleHandle>& p1s,
				     const std::vector<ParticleHandle>& p2s);

  /// returns the ParticleHandles from ps that are in a cone around lv.
  std::vector<ParticleHandle> inCone(const LorentzVector& lv,
				     const std::vector<ParticleHandle>& ps,
				     float deltaRMax,
				     float exclusion=1e-5);

  /// returns the scalar sum pT of the particles in ps
  float sumPt(const std::vector<ParticleHandle>& ps);

  /// returns the scalar sum p of the particles in ps
  float sumP(const std::vector<ParticleHandle>& ps);


}

std::ostream& operator<<(std::ostream& out, const ParticleHandle& ptc);

#endif
