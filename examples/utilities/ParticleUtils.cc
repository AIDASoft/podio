#include "ParticleUtils.h"

#include "ParticleHandle.h"
#include "LorentzVector.h"

#include "VectorUtils.h"

#include "TLorentzVector.h"
#include <set>
#include <iterator>

namespace utils {


  std::vector<ParticleHandle> unused(const std::vector<ParticleHandle>& p1s,
				     const std::vector<ParticleHandle>& p2s) {
    std::vector<ParticleHandle> results;
    std::set<ParticleHandle> p2set;
    std::copy( p2s.begin(), p2s.end(),
	       std::inserter( p2set, p2set.end() ) );
    for(const auto& particle : p1s) {
      if( p2set.find(particle) == p2set.end() ) {
	results.push_back(particle);
      }
    }
    return results;
  }


  std::vector<ParticleHandle> inCone(const LorentzVector& lv,
				     const std::vector<ParticleHandle>& ps,
				     float deltaRMax) {
    float dR2Max = deltaRMax*deltaRMax;
    std::vector<ParticleHandle> results;
    for(const auto& particle : ps) {
      float dR2 = deltaR2(lv, particle.P4());
      if( dR2 <= dR2Max ) {
	results.emplace_back(particle);
      }
    }
    return results;
  }


  float sumPt(const std::vector<ParticleHandle>& ps) {
    float result = 0;
    for(const auto& particle : ps) {
      result += particle.P4().Pt;
    }
    return result;
  }


  float sumP(const std::vector<ParticleHandle>& ps) {
    float result = 0;
    for(const auto& particle : ps) {
      TLorentzVector lv = lvFromPOD( particle.P4() );
      result += lv.Vect().Mag();
    }
    return result;
  }


} // namespace
