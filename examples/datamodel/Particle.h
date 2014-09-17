#ifndef Particle_H 
#define Particle_H


class ParticleCollection;

class Particle {
  friend ParticleCollection;

public:
  const float& Pz() const { return m_Pz;};
  const float& Px() const { return m_Px;};
  const float& Py() const { return m_Py;};
  const float& Energy() const { return m_Energy;};
  const int& Charge() const { return m_Charge;};

  void setPz(float& value){ m_Pz = value;};
  void setPx(float& value){ m_Px = value;};
  void setPy(float& value){ m_Py = value;};
  void setEnergy(float& value){ m_Energy = value;};
  void setCharge(int& value){ m_Charge = value;};


private:
  float m_Pz;
  float m_Px;
  float m_Py;
  float m_Energy;
  int m_Charge;

};

#endif