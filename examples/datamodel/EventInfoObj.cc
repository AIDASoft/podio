#include "EventInfoObj.h"

EventInfoObj::EventInfoObj() :
    data(),
    id{albers::ObjectID::untracked,albers::ObjectID::untracked},
    ref_counter(1) { }

EventInfoObj::EventInfoObj(const albers::ObjectID id, EventInfoData data) :
    data(data),
    id(id),
    ref_counter(-1) { }

EventInfoObj::EventInfoObj(const EventInfoObj& other) :
    data(),
    id{albers::ObjectID::untracked,albers::ObjectID::untracked},
    ref_counter(0) { }

EventInfoObj::~EventInfoObj() {


}
