#include "ExampleForCyclicDependency1Obj.h"
#include "ExampleForCyclicDependency2Const.h"



ExampleForCyclicDependency1Obj::ExampleForCyclicDependency1Obj() :
    ObjBase{{podio::ObjectID::untracked,podio::ObjectID::untracked},0}
    ,data()
    ,m_ref(nullptr)

    { }

ExampleForCyclicDependency1Obj::ExampleForCyclicDependency1Obj(const podio::ObjectID id, ExampleForCyclicDependency1Data data) :
    ObjBase{id,0},
    data(data)
    { }

ExampleForCyclicDependency1Obj::ExampleForCyclicDependency1Obj(const ExampleForCyclicDependency1Obj& other) :
    ObjBase{{podio::ObjectID::untracked,podio::ObjectID::untracked},0}
    ,data(other.data)
    
    { }

ExampleForCyclicDependency1Obj::~ExampleForCyclicDependency1Obj() {
  if (id.index == podio::ObjectID::untracked) {
delete m_ref;

  }
}

