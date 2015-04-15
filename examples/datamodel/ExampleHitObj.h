#ifndef ExampleHitOBJ_H
#define ExampleHitOBJ_H

// std includes
#include <atomic>

// data model specific includes
#include "albers/ObjectID.h"
#include "ExampleHitData.h"



// forward declarations
class ExampleHit;

class ExampleHitObj {
public:
  ExampleHitObj();
  ExampleHitObj(const ExampleHitObj&); //TODO: deep copy!
  ExampleHitObj(const albers::ObjectID id, ExampleHitData data);
  ~ExampleHitObj();
  void increaseRefCount() {
    if (id.index == albers::ObjectID::untracked) ++ref_counter;
  };

  int decreaseRefCount(){
    return  ( (id.index == albers::ObjectID::untracked) ? --ref_counter : 1 );
  }; // returns current count

public:
  ExampleHitData data;

  albers::ObjectID id;
  
private:
  std::atomic<int> ref_counter;
};


#endif
