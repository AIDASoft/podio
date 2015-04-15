#ifndef ExampleClusterOBJ_H
#define ExampleClusterOBJ_H

// std includes
#include <atomic>

// data model specific includes
#include "albers/ObjectID.h"
#include "ExampleClusterData.h"

#include "ExampleHit.h"


// forward declarations
class ExampleCluster;

class ExampleClusterObj {
public:
  ExampleClusterObj();
  ExampleClusterObj(const ExampleClusterObj&); //TODO: deep copy!
  ExampleClusterObj(const albers::ObjectID id, ExampleClusterData data);
  ~ExampleClusterObj();
  void increaseRefCount() {
    if (id.index == albers::ObjectID::untracked) ++ref_counter;
  };

  int decreaseRefCount(){
    return  ( (id.index == albers::ObjectID::untracked) ? --ref_counter : 1 );
  }; // returns current count

public:
  ExampleClusterData data;
  std::vector<ExampleHit>* m_Hits;

  albers::ObjectID id;
  
private:
  std::atomic<int> ref_counter;
};


#endif
