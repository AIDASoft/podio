#include "ParticleUtils.h"

#include "ParticleHandle.h"
#include "LorentzVector.h"

#include "VectorUtils.h"

#include "TLorentzVector.h"
#include <set>
#include <iterator>
#include <iostream>

namespace utils {


  std::deque<ParticleHandle> unused(const std::deque<ParticleHandle>& p1s,
				     const std::deque<ParticleHandle>& p2s) {
    std::deque<ParticleHandle> results;
    std::set<ParticleHandle> p2set;
    std::copy( p2s.begin(), p2s.end(),
	       std::inserter( p2set, p2set.end() ) );
    //    std::cout<<"set"<<std::endl;
    // for(const auto& particle : p2set) {
    //  std::cout<<particle.containerID()<<" "<<particle.index()<<std::endl;
    // }
    // std::cout<<"particles"<<std::endl;
    for(const auto& particle : p1s) {
      // std::cout<<particle.containerID()<<" "<<particle.index()<<std::endl;
      if( p2set.find(particle) == p2set.end() ) {
	// std::cout<<"not found"<<std::endl;
	results.push_back(particle);
      }
    }
    return results;
  }


  std::deque<ParticleHandle> inCone(const LorentzVector& lv,
				     const std::deque<ParticleHandle>& ps,
				     float deltaRMax,
				     float exclusion ) {
    float dR2Max = deltaRMax*deltaRMax;
    float exc2 = exclusion*exclusion;
    std::deque<ParticleHandle> results;
    for(const auto& particle : ps) {
      float dR2 = deltaR2(lv, particle.P4());
      if( dR2>exc2 && dR2 <= dR2Max ) {
	results.emplace_back(particle);
      }
    }
    return results;
  }


  float sumPt(const std::deque<ParticleHandle>& ps) {
    float result = 0;
    for(const auto& particle : ps) {
      result += particle.P4().Pt;
    }
    return result;
  }


  float sumP(const std::deque<ParticleHandle>& ps) {
    float result = 0;
    for(const auto& particle : ps) {
      TLorentzVector lv = lvFromPOD( particle.P4() );
      result += lv.Vect().Mag();
    }
    return result;
  }


} // namespace

std::ostream& operator<<(std::ostream& out, const ParticleHandle& ptc) {
  if(not out) return out;
  out<<"particle ID "<<ptc.ID()
     <<" pt "<<ptc.P4().Pt
     <<" eta "<<ptc.P4().Eta
     <<" phi "<<ptc.P4().Phi;
  return out;
}
