#ifndef ExampleReferencingTypeENTRY_H
#define ExampleReferencingTypeENTRY_H

// std includes
#include <atomic>

// datamodel specific includes
#include "albers/ObjectID.h"
#include "ExampleReferencingTypeData.h"

#include "ExampleCluster.h"


class ExampleReferencingTypeEntry {
public:
  ExampleReferencingTypeEntry();
  ExampleReferencingTypeEntry(const ExampleReferencingTypeEntry&); //TODO: deep copy!
  ExampleReferencingTypeEntry(const albers::ObjectID id, ExampleReferencingTypeData data);
  ~ExampleReferencingTypeEntry();
  void increaseRefCount() {
    if (id.index == albers::ObjectID::untracked) ++ref_counter;
  }
;
  int decreaseRefCount(){
    return  ( (id.index == albers::ObjectID::untracked) ? --ref_counter : 1 );
  }; // returns current count

public:
  ExampleReferencingTypeData data;
  std::vector<ExampleCluster>* m_Clusters;

  albers::ObjectID id;
  std::atomic<int> ref_counter;
};


#endif
