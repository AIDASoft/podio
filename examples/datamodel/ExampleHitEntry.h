#ifndef ExampleHitENTRY_H
#define ExampleHitENTRY_H

// std includes
#include <atomic>

// datamodel specific includes
#include "albers/ObjectID.h"
#include "ExampleHitData.h"



class ExampleHitEntry {
public:
  ExampleHitEntry();
  ExampleHitEntry(const ExampleHitEntry&); //TODO: deep copy!
  ExampleHitEntry(const albers::ObjectID id, ExampleHitData data);
  ~ExampleHitEntry();
  void increaseRefCount() {
    if (id.index == albers::ObjectID::untracked) ++ref_counter;
  }
;
  int decreaseRefCount(){
    return  ( (id.index == albers::ObjectID::untracked) ? --ref_counter : 1 );
  }; // returns current count

public:
  ExampleHitData data;

  albers::ObjectID id;
  std::atomic<int> ref_counter;
};


#endif
