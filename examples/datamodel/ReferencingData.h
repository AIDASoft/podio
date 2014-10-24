#ifndef ReferencingData_H 
#define ReferencingData_H

// ref to dummy data
// author: C. Bernet, B. Hegner

#include "DummyDataHandle.h"


class ReferencingData {
public:
  DummyDataHandle Dummy; //ref to dummy data 
  unsigned int Dummies_begin; 
  unsigned Dummies_end; 

};

#endif