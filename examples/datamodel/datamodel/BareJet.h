#ifndef BareJet_H 
#define BareJet_H

#include "datamodel/LorentzVector.h"


class BareJet {
public:
  unsigned Bits;
  LorentzVector P4;
  float Area;

};

#endif