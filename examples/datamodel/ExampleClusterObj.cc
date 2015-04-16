#include "ExampleClusterObj.h"

ExampleClusterObj::ExampleClusterObj() :
    ObjBase{{albers::ObjectID::untracked,albers::ObjectID::untracked},1},
    data()
    { }

ExampleClusterObj::ExampleClusterObj(const albers::ObjectID id, ExampleClusterData data) :
    ObjBase{id,-1},
    data(data)
    { }

ExampleClusterObj::ExampleClusterObj(const ExampleClusterObj& other) :
    ObjBase{{albers::ObjectID::untracked,albers::ObjectID::untracked},0},
    data()
    { }

ExampleClusterObj::~ExampleClusterObj() {
  if (id.index == albers::ObjectID::untracked) {
delete m_Hits;

  }
}
