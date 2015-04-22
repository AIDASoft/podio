#include "ExampleWithVectorMemberObj.h"

ExampleWithVectorMemberObj::ExampleWithVectorMemberObj() :
    ObjBase{{albers::ObjectID::untracked,albers::ObjectID::untracked},0}
    ,data()
    
    { }

ExampleWithVectorMemberObj::ExampleWithVectorMemberObj(const albers::ObjectID id, ExampleWithVectorMemberData data) :
    ObjBase{id,0},
    data(data)
    { }

ExampleWithVectorMemberObj::ExampleWithVectorMemberObj(const ExampleWithVectorMemberObj& other) :
    ObjBase{{albers::ObjectID::untracked,albers::ObjectID::untracked},0}
    ,data(other.data)
    
    { }

ExampleWithVectorMemberObj::~ExampleWithVectorMemberObj() {
  if (id.index == albers::ObjectID::untracked) {

  }
}
