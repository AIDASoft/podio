#ifndef ExampleHitOBJ_H
#define ExampleHitOBJ_H

// std includes
#include <atomic>
#include <iostream>

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
    if (id.index != albers::ObjectID::untracked){ return 1;};
    if (--ref_counter == 0) {
      std::cout << "deleting free-floating ExampleHit at " << this << std::endl;
      delete this;
    }
    return 0;
    //return  ( (id.index == albers::ObjectID::untracked) ? --ref_counter : 1 );
  }; // returns current count

public:
  ExampleHitData data;

  albers::ObjectID id;

private:
  std::atomic<int> ref_counter;
};


#endif
