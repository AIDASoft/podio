#include "ExampleWithArrayObj.h"

ExampleWithArrayObj::ExampleWithArrayObj()
    : ObjBase{{podio::ObjectID::untracked, podio::ObjectID::untracked}, 0},
      data() {}

ExampleWithArrayObj::ExampleWithArrayObj(const podio::ObjectID id,
                                         ExampleWithArrayData data)
    : ObjBase{id, 0}, data(data) {}

ExampleWithArrayObj::ExampleWithArrayObj(const ExampleWithArrayObj &other)
    : ObjBase{{podio::ObjectID::untracked, podio::ObjectID::untracked}, 0},
      data(other.data) {}

ExampleWithArrayObj::~ExampleWithArrayObj() {
  if (id.index == podio::ObjectID::untracked) {
  }
}
