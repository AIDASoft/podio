#include "ExampleWithOneRelationObj.h"

ExampleWithOneRelationObj::ExampleWithOneRelationObj() :
    ObjBase{{albers::ObjectID::untracked,albers::ObjectID::untracked},0}
    ,data()
    
    { }

ExampleWithOneRelationObj::ExampleWithOneRelationObj(const albers::ObjectID id, ExampleWithOneRelationData data) :
    ObjBase{id,0},
    data(data)
    { }

ExampleWithOneRelationObj::ExampleWithOneRelationObj(const ExampleWithOneRelationObj& other) :
    ObjBase{{albers::ObjectID::untracked,albers::ObjectID::untracked},0}
    ,data(other.data)
    
    { }

ExampleWithOneRelationObj::~ExampleWithOneRelationObj() {
  if (id.index == albers::ObjectID::untracked) {

  }
}
