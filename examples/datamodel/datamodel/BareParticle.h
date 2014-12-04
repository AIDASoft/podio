#ifndef BareParticle_H 
#define BareParticle_H

#include "datamodel/Point.h"
#include "datamodel/LorentzVector.h"


class BareParticle {
public:
  unsigned Status;
  unsigned Bits;
  int Type;
  Point Vertex;
  LorentzVector P4;

};

#endif