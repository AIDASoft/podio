#ifndef ExampleClusterOBJ_H
#define ExampleClusterOBJ_H

// std includes
#include <atomic>
#include <iostream>

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
    if (id.index != albers::ObjectID::untracked){ return 1;};
    if (--ref_counter == 0) {
      std::cout << "deleting free-floating ExampleCluster at " << this << std::endl;
      delete this;
    }
    return 0;
    //return  ( (id.index == albers::ObjectID::untracked) ? --ref_counter : 1 );
  }; // returns current count

public:
  ExampleClusterData data;
  std::vector<ExampleHit>* m_Hits;

  albers::ObjectID id;

private:
  std::atomic<int> ref_counter;
};


#endif
