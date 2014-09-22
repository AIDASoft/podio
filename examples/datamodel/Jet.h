#ifndef Jet_H 
#define Jet_H
#include "LorentzVectorHandle.h"


class JetCollection;

class Jet {
  friend JetCollection;

public:
  const LorentzVectorHandle& P4() const { return m_P4;};

  void setP4(LorentzVectorHandle& value){ m_P4 = value;};


private:
  LorentzVectorHandle m_P4;

};

#endif