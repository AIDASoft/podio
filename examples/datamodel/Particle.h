#ifndef Particle_H 
#define Particle_H
#include "LorentzVector.h"


class ParticleCollection;

class Particle {
  friend ParticleCollection;

public:
  const int& ID() const { return m_ID;};
  const LorentzVector& P4() const { return m_P4;};

  void setID(int& value){ m_ID = value;};
  void setP4(LorentzVector& value){ m_P4 = value;};


private:
  int m_ID;
  LorentzVector m_P4;

};

#endif