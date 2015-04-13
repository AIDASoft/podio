#include "ExampleClusterEntry.h"

ExampleClusterEntry::ExampleClusterEntry() :
    data(),
    id{albers::ObjectID::untracked,albers::ObjectID::untracked},
    ref_counter(1) { }

ExampleClusterEntry::ExampleClusterEntry(const albers::ObjectID id, ExampleClusterData data) :
    data(data),
    id(id),
    ref_counter(-1) { }

ExampleClusterEntry::ExampleClusterEntry(const ExampleClusterEntry& other) :
    data(),
    id{albers::ObjectID::untracked,albers::ObjectID::untracked},
    ref_counter(0) { }

ExampleClusterEntry::~ExampleClusterEntry() {


}
