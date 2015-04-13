#include "ExampleHitEntry.h"

ExampleHitEntry::ExampleHitEntry() :
    data(),
    id{albers::ObjectID::untracked,albers::ObjectID::untracked},
    ref_counter(1) { }

ExampleHitEntry::ExampleHitEntry(const albers::ObjectID id, ExampleHitData data) :
    data(data),
    id(id),
    ref_counter(-1) { }

ExampleHitEntry::ExampleHitEntry(const ExampleHitEntry& other) :
    data(),
    id{albers::ObjectID::untracked,albers::ObjectID::untracked},
    ref_counter(0) { }

ExampleHitEntry::~ExampleHitEntry() {


}
