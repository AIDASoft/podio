#ifndef JetParticle_H
#define JetParticle_H
#include "JetHandle.h"
#include "ParticleHandle.h"

class JetParticleCollection;

class JetParticle {
  friend JetParticleCollection;

public:
  const JetHandle& Jet() const { return m_Jet;};
  const ParticleHandle& Particle() const { return m_Particle;};

  void setJet(JetHandle& value){ m_Jet = value;};
  void setParticle(ParticleHandle& value){ m_Particle = value;};


private:
  JetHandle m_Jet;
  ParticleHandle m_Particle;

};

#endif
