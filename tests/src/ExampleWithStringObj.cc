#include "ExampleWithStringObj.h"

ExampleWithStringObj::ExampleWithStringObj()
    : ObjBase{{podio::ObjectID::untracked, podio::ObjectID::untracked}, 0},
      data() {}

ExampleWithStringObj::ExampleWithStringObj(const podio::ObjectID id,
                                           ExampleWithStringData data)
    : ObjBase{id, 0}, data(data) {}

ExampleWithStringObj::ExampleWithStringObj(const ExampleWithStringObj &other)
    : ObjBase{{podio::ObjectID::untracked, podio::ObjectID::untracked}, 0},
      data(other.data) {}

ExampleWithStringObj::~ExampleWithStringObj() {
  if (id.index == podio::ObjectID::untracked) {
  }
}
