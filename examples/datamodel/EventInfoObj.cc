#include "EventInfoObj.h"

EventInfoObj::EventInfoObj() :
    ObjBase{{albers::ObjectID::untracked,albers::ObjectID::untracked},1},
    data()
    
    { }

EventInfoObj::EventInfoObj(const albers::ObjectID id, EventInfoData data) :
    ObjBase{id,-1},
    data(data)
    { }

EventInfoObj::EventInfoObj(const EventInfoObj& other) :
    ObjBase{{albers::ObjectID::untracked,albers::ObjectID::untracked},0},
    data(other.data)
    
    { }

EventInfoObj::~EventInfoObj() {
  if (id.index == albers::ObjectID::untracked) {

  }
}
