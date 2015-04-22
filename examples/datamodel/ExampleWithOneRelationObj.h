#ifndef ExampleWithOneRelationOBJ_H
#define ExampleWithOneRelationOBJ_H

// std includes
#include <atomic>
#include <iostream>

// data model specific includes
#include "albers/ObjBase.h"
#include "ExampleWithOneRelationData.h"

#include "ExampleCluster.h"


// forward declarations
class ExampleWithOneRelation;

class ExampleWithOneRelationObj : public albers::ObjBase {
public:
  /// constructor
  ExampleWithOneRelationObj();
  /// copy constructor (does a deep-copy of relation containers)
  ExampleWithOneRelationObj(const ExampleWithOneRelationObj&);
  /// constructor from ObjectID and ExampleWithOneRelationData
  /// does not initialize the internal relation containers
  ExampleWithOneRelationObj(const albers::ObjectID id, ExampleWithOneRelationData data);
  virtual ~ExampleWithOneRelationObj();

public:
  ExampleWithOneRelationData data;
  ExampleCluster m_cluster;


};


#endif
