#ifndef ExampleReferencingTypeOBJ_H
#define ExampleReferencingTypeOBJ_H

// std includes
#include <atomic>

// data model specific includes
#include "albers/ObjectID.h"
#include "ExampleReferencingTypeData.h"

#include "ExampleCluster.h"
#include "ExampleReferencingType.h"


// forward declarations
class ExampleReferencingType;

class ExampleReferencingTypeObj {
public:
  ExampleReferencingTypeObj();
  ExampleReferencingTypeObj(const ExampleReferencingTypeObj&); //TODO: deep copy!
  ExampleReferencingTypeObj(const albers::ObjectID id, ExampleReferencingTypeData data);
  ~ExampleReferencingTypeObj();
  void increaseRefCount() {
    if (id.index == albers::ObjectID::untracked) ++ref_counter;
  };

  int decreaseRefCount(){
    return  ( (id.index == albers::ObjectID::untracked) ? --ref_counter : 1 );
  }; // returns current count

public:
  ExampleReferencingTypeData data;
  std::vector<ExampleCluster>* m_Clusters;
  std::vector<ExampleReferencingType>* m_Refs;

  albers::ObjectID id;
  
private:
  std::atomic<int> ref_counter;
};


#endif
