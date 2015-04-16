#ifndef ExampleClusterOBJ_H
#define ExampleClusterOBJ_H

// std includes
#include <atomic>
#include <iostream>

// data model specific includes
#include "albers/ObjBase.h"
#include "ExampleClusterData.h"

#include "ExampleHit.h"


// forward declarations
class ExampleCluster;

class ExampleClusterObj : public albers::ObjBase {
public:
  ExampleClusterObj();
  ExampleClusterObj(const ExampleClusterObj&); //TODO: deep copy!
  ExampleClusterObj(const albers::ObjectID id, ExampleClusterData data);
  virtual ~ExampleClusterObj();

public:
  ExampleClusterData data;
  std::vector<ExampleHit>* m_Hits;


};


#endif
