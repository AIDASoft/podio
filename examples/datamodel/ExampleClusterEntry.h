#ifndef ExampleClusterENTRY_H
#define ExampleClusterENTRY_H

// std includes
#include <atomic>

// datamodel specific includes
#include "albers/ObjectID.h"
#include "ExampleClusterData.h"

#include "ExampleHit.h"


class ExampleClusterEntry {
public:
  ExampleClusterEntry();
  ExampleClusterEntry(const ExampleClusterEntry&); //TODO: deep copy!
  ExampleClusterEntry(const albers::ObjectID id, ExampleClusterData data);
  ~ExampleClusterEntry();
  void increaseRefCount() {
    if (id.index == albers::ObjectID::untracked) ++ref_counter;
  }
;
  int decreaseRefCount(){
    return  ( (id.index == albers::ObjectID::untracked) ? --ref_counter : 1 );
  }; // returns current count

public:
  ExampleClusterData data;
  std::vector<ExampleHit>* m_Hits;

  albers::ObjectID id;
  std::atomic<int> ref_counter;
};


#endif
