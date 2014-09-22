#ifndef Particle_H 
#define Particle_H
#include "LorentzVectorHandle.h"


class ParticleCollection;

class Particle {
  friend ParticleCollection;

public:
  const int& ID() const { return m_ID;};
  const LorentzVectorHandle& P4() const { return m_P4;};

  void setID(int& value){ m_ID = value;};
  void setP4(LorentzVectorHandle& value){ m_P4 = value;};


private:
  int m_ID;
  LorentzVectorHandle m_P4;

};

#endif