#ifndef ExampleReferencingTypeOBJ_H
#define ExampleReferencingTypeOBJ_H

// std includes
#include <atomic>
#include <iostream>

// data model specific includes
#include "albers/ObjBase.h"
#include "ExampleReferencingTypeData.h"

#include <vector>
#include "ExampleCluster.h"
#include "ExampleReferencingType.h"


// forward declarations
class ExampleReferencingType;

class ExampleReferencingTypeObj : public albers::ObjBase {
public:
  /// constructor
  ExampleReferencingTypeObj();
  /// copy constructor (does a deep-copy of relation containers)
  ExampleReferencingTypeObj(const ExampleReferencingTypeObj&);
  /// constructor from ObjectID and ExampleReferencingTypeData
  /// does not initialize the internal relation containers
  ExampleReferencingTypeObj(const albers::ObjectID id, ExampleReferencingTypeData data);
  virtual ~ExampleReferencingTypeObj();

public:
  ExampleReferencingTypeData data;
  std::vector<ExampleCluster>* m_Clusters;
  std::vector<ExampleReferencingType>* m_Refs;


};


#endif
