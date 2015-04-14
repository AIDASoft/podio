#include "ExampleReferencingTypeObj.h"

ExampleReferencingTypeObj::ExampleReferencingTypeObj() :
    data(),
    id{albers::ObjectID::untracked,albers::ObjectID::untracked},
    ref_counter(1) { }

ExampleReferencingTypeObj::ExampleReferencingTypeObj(const albers::ObjectID id, ExampleReferencingTypeData data) :
    data(data),
    id(id),
    ref_counter(-1) { }

ExampleReferencingTypeObj::ExampleReferencingTypeObj(const ExampleReferencingTypeObj& other) :
    data(),
    id{albers::ObjectID::untracked,albers::ObjectID::untracked},
    ref_counter(0) { }

ExampleReferencingTypeObj::~ExampleReferencingTypeObj() {


}
