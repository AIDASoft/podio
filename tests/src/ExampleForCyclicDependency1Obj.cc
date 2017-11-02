#include "ExampleForCyclicDependency1Obj.h"
#include "ExampleForCyclicDependency2Const.h"

ExampleForCyclicDependency1Obj::ExampleForCyclicDependency1Obj()
    : ObjBase{{podio::ObjectID::untracked, podio::ObjectID::untracked}, 0},
      data(), m_ref(nullptr)

{}

ExampleForCyclicDependency1Obj::ExampleForCyclicDependency1Obj(
    const podio::ObjectID id, ExampleForCyclicDependency1Data data)
    : ObjBase{id, 0}, data(data) {}

ExampleForCyclicDependency1Obj::ExampleForCyclicDependency1Obj(
    const ExampleForCyclicDependency1Obj &other)
    : ObjBase{{podio::ObjectID::untracked, podio::ObjectID::untracked}, 0},
      data(other.data), m_ref(nullptr) {
  if (other.m_ref != nullptr) {
    m_ref = new ConstExampleForCyclicDependency2(*(other.m_ref));
  }
}

ExampleForCyclicDependency1Obj::~ExampleForCyclicDependency1Obj() {
  if (id.index == podio::ObjectID::untracked) {
  }
  if (m_ref != nullptr)
    delete m_ref;
}
