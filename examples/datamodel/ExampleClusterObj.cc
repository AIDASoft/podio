#include "ExampleClusterObj.h"

ExampleClusterObj::ExampleClusterObj() :
    data(),
    id{albers::ObjectID::untracked,albers::ObjectID::untracked},
    ref_counter(1) { }

ExampleClusterObj::ExampleClusterObj(const albers::ObjectID id, ExampleClusterData data) :
    data(data),
    id(id),
    ref_counter(-1) { }

ExampleClusterObj::ExampleClusterObj(const ExampleClusterObj& other) :
    data(),
    id{albers::ObjectID::untracked,albers::ObjectID::untracked},
    ref_counter(0) { }

ExampleClusterObj::~ExampleClusterObj() {


}
