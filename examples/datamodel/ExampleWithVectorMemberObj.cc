#include "ExampleWithVectorMemberObj.h"

ExampleWithVectorMemberObj::ExampleWithVectorMemberObj() :
    ObjBase{{podio::ObjectID::untracked,podio::ObjectID::untracked},0}
    ,data()
    
    { }

ExampleWithVectorMemberObj::ExampleWithVectorMemberObj(const podio::ObjectID id, ExampleWithVectorMemberData data) :
    ObjBase{id,0},
    data(data)
    { }

ExampleWithVectorMemberObj::ExampleWithVectorMemberObj(const ExampleWithVectorMemberObj& other) :
    ObjBase{{podio::ObjectID::untracked,podio::ObjectID::untracked},0}
    ,data(other.data)
    
    { }

ExampleWithVectorMemberObj::~ExampleWithVectorMemberObj() {
  if (id.index == podio::ObjectID::untracked) {

  }
}
