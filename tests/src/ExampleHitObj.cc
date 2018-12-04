#include "ExampleHitObj.h"

ExampleHitObj::ExampleHitObj()
    : ObjBase{{podio::ObjectID::untracked, podio::ObjectID::untracked}, 0},
      data() {}

ExampleHitObj::ExampleHitObj(const podio::ObjectID id, ExampleHitData data)
    : ObjBase{id, 0}, data(data) {}

ExampleHitObj::ExampleHitObj(const ExampleHitObj &other)
    : ObjBase{{podio::ObjectID::untracked, podio::ObjectID::untracked}, 0},
      data(other.data) {}

ExampleHitObj::~ExampleHitObj() {
  if (id.index == podio::ObjectID::untracked) {
  }
}
