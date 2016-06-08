#include "ExampleForCyclicDependency2Obj.h"
#include "ExampleForCyclicDependency1.h"



ExampleForCyclicDependency2Obj::ExampleForCyclicDependency2Obj() :
    ObjBase{{podio::ObjectID::untracked,podio::ObjectID::untracked},0}, data(),m_ref(nullptr)

{ }

ExampleForCyclicDependency2Obj::ExampleForCyclicDependency2Obj(const podio::ObjectID id, ExampleForCyclicDependency2Data data) :
    ObjBase{id,0}, data(data)
{ }

ExampleForCyclicDependency2Obj::ExampleForCyclicDependency2Obj(const ExampleForCyclicDependency2Obj& other) :
    ObjBase{{podio::ObjectID::untracked,podio::ObjectID::untracked},0}
    , data(other.data)
{ }

ExampleForCyclicDependency2Obj::~ExampleForCyclicDependency2Obj() {
  if (id.index == podio::ObjectID::untracked) {

  }
    if (m_ref != nullptr) delete m_ref;

}

