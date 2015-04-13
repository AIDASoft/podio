#include "EventInfoEntry.h"

EventInfoEntry::EventInfoEntry() :
    data(),
    id{albers::ObjectID::untracked,albers::ObjectID::untracked},
    ref_counter(1) { }

EventInfoEntry::EventInfoEntry(const albers::ObjectID id, EventInfoData data) :
    data(data),
    id(id),
    ref_counter(-1) { }

EventInfoEntry::EventInfoEntry(const EventInfoEntry& other) :
    data(),
    id{albers::ObjectID::untracked,albers::ObjectID::untracked},
    ref_counter(0) { }

EventInfoEntry::~EventInfoEntry() {


}
