#ifndef Jet_H 
#define Jet_H

// 
// author: C. Bernet, B. Hegner

#include "LorentzVector.h"


class Jet {
public:
  LorentzVector P4; // 
  unsigned int particles_begin; 
  unsigned particles_end; 

};

#endif