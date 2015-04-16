#include "ExampleHitObj.h"

ExampleHitObj::ExampleHitObj() :
    ObjBase{{albers::ObjectID::untracked,albers::ObjectID::untracked},1},
    data()
    { }

ExampleHitObj::ExampleHitObj(const albers::ObjectID id, ExampleHitData data) :
    ObjBase{id,-1},
    data(data)
    { }

ExampleHitObj::ExampleHitObj(const ExampleHitObj& other) :
    ObjBase{{albers::ObjectID::untracked,albers::ObjectID::untracked},0},
    data()
    { }

ExampleHitObj::~ExampleHitObj() {


}
