#ifndef UTILS_PARTICLE_H
#define UTILS_PARTICLE_H

#include <deque>
#include <iostream>

class ParticleHandle;
class LorentzVector;

namespace utils {

  /// returns the ParticleHandles that are in p1s but not in p2s.
  ///
  /// This algorithm preserves ordering and possible duplications in p1s.
  std::deque<ParticleHandle> unused(const std::deque<ParticleHandle>& p1s,
				     const std::deque<ParticleHandle>& p2s);

  /// returns the ParticleHandles from ps that are in a cone around lv.
  std::deque<ParticleHandle> inCone(const LorentzVector& lv,
				     const std::deque<ParticleHandle>& ps,
				     float deltaRMax,
				     float exclusion=1e-5);

  /// returns the scalar sum pT of the particles in ps
  float sumPt(const std::deque<ParticleHandle>& ps);

  /// returns the scalar sum p of the particles in ps
  float sumP(const std::deque<ParticleHandle>& ps);


}

std::ostream& operator<<(std::ostream& out, const ParticleHandle& ptc);

#endif
