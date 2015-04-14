#include "ExampleHitObj.h"

ExampleHitObj::ExampleHitObj() :
    data(),
    id{albers::ObjectID::untracked,albers::ObjectID::untracked},
    ref_counter(1) { }

ExampleHitObj::ExampleHitObj(const albers::ObjectID id, ExampleHitData data) :
    data(data),
    id(id),
    ref_counter(-1) { }

ExampleHitObj::ExampleHitObj(const ExampleHitObj& other) :
    data(),
    id{albers::ObjectID::untracked,albers::ObjectID::untracked},
    ref_counter(0) { }

ExampleHitObj::~ExampleHitObj() {


}
