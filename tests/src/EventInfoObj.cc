#include "EventInfoObj.h"

EventInfoObj::EventInfoObj()
    : ObjBase{{podio::ObjectID::untracked, podio::ObjectID::untracked}, 0},
      data() {}

EventInfoObj::EventInfoObj(const podio::ObjectID id, EventInfoData data)
    : ObjBase{id, 0}, data(data) {}

EventInfoObj::EventInfoObj(const EventInfoObj &other)
    : ObjBase{{podio::ObjectID::untracked, podio::ObjectID::untracked}, 0},
      data(other.data) {}

EventInfoObj::~EventInfoObj() {
  if (id.index == podio::ObjectID::untracked) {
  }
}
