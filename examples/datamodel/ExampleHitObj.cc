#include "ExampleHitObj.h"

ExampleHitObj::ExampleHitObj() :
    ObjBase{{albers::ObjectID::untracked,albers::ObjectID::untracked},0},
    data()
    
    { }

ExampleHitObj::ExampleHitObj(const albers::ObjectID id, ExampleHitData data) :
    ObjBase{id,0},
    data(data)
    { }

ExampleHitObj::ExampleHitObj(const ExampleHitObj& other) :
    ObjBase{{albers::ObjectID::untracked,albers::ObjectID::untracked},0},
    data(other.data)
    
    { }

ExampleHitObj::~ExampleHitObj() {
  if (id.index == albers::ObjectID::untracked) {

  }
}
