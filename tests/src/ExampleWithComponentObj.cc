#include "ExampleWithComponentObj.h"

ExampleWithComponentObj::ExampleWithComponentObj()
    : ObjBase{{podio::ObjectID::untracked, podio::ObjectID::untracked}, 0},
      data() {}

ExampleWithComponentObj::ExampleWithComponentObj(const podio::ObjectID id,
                                                 ExampleWithComponentData data)
    : ObjBase{id, 0}, data(data) {}

ExampleWithComponentObj::ExampleWithComponentObj(
    const ExampleWithComponentObj &other)
    : ObjBase{{podio::ObjectID::untracked, podio::ObjectID::untracked}, 0},
      data(other.data) {}

ExampleWithComponentObj::~ExampleWithComponentObj() {
  if (id.index == podio::ObjectID::untracked) {
  }
}
