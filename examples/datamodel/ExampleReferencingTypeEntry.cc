#include "ExampleReferencingTypeEntry.h"

ExampleReferencingTypeEntry::ExampleReferencingTypeEntry() :
    data(),
    id{albers::ObjectID::untracked,albers::ObjectID::untracked},
    ref_counter(1) { }

ExampleReferencingTypeEntry::ExampleReferencingTypeEntry(const albers::ObjectID id, ExampleReferencingTypeData data) :
    data(data),
    id(id),
    ref_counter(-1) { }

ExampleReferencingTypeEntry::ExampleReferencingTypeEntry(const ExampleReferencingTypeEntry& other) :
    data(),
    id{albers::ObjectID::untracked,albers::ObjectID::untracked},
    ref_counter(0) { }

ExampleReferencingTypeEntry::~ExampleReferencingTypeEntry() {


}
