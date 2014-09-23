#ifndef ParticleRef_H 
#define ParticleRef_H
#include "ParticleHandle.h"


class ParticleRefCollection;

class ParticleRef {
  friend ParticleRefCollection;

public:
  const ParticleHandle& Particle() const { return m_Particle;};

  void setParticle(ParticleHandle& value){ m_Particle = value;};


private:
  ParticleHandle m_Particle;

};

#endif