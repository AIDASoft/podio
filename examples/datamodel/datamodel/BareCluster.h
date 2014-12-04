#ifndef BareCluster_H 
#define BareCluster_H

#include "datamodel/Point.h"


class BareCluster {
public:
  Point position;
  float Energy;
  unsigned Bits;
  float Time;

};

#endif