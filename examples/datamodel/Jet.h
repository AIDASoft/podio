#ifndef Jet_H 
#define Jet_H
#include "LorentzVector.h"


class JetCollection;

class Jet {
  friend JetCollection;

public:
  const LorentzVector& P4() const { return m_P4;};

  void setP4(LorentzVector& value){ m_P4 = value;};


private:
  LorentzVector m_P4;

};

#endif