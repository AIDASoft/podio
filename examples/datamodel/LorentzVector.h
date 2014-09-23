#ifndef LorentzVector_H
#define LorentzVector_H


class LorentzVectorCollection;

class LorentzVector {
  friend LorentzVectorCollection;

public:
  // COLIN code generator must respect the naming scheme
  const float& Phi() const { return m_Phi;};
  const float& Eta() const { return m_Eta;};
  const float& Mass() const { return m_Mass;};
  const float& Pt() const { return m_Pt;};

  void setPhi(float& value){ m_Phi = value;};
  void setEta(float& value){ m_Eta = value;};
  void setMass(float& value){ m_Mass = value;};
  void setPt(float& value){ m_Pt = value;};


private:
  float m_Phi;
  float m_Eta;
  float m_Mass;
  float m_Pt;

};

#endif
