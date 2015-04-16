#ifndef ExampleHitOBJ_H
#define ExampleHitOBJ_H

// std includes
#include <atomic>
#include <iostream>

// data model specific includes
#include "albers/ObjBase.h"
#include "ExampleHitData.h"



// forward declarations
class ExampleHit;

class ExampleHitObj : public albers::ObjBase {
public:
  ExampleHitObj();
  ExampleHitObj(const ExampleHitObj&); //TODO: deep copy!
  ExampleHitObj(const albers::ObjectID id, ExampleHitData data);
  virtual ~ExampleHitObj();

public:
  ExampleHitData data;


};


#endif
