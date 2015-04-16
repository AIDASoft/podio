#include "ExampleReferencingTypeObj.h"

ExampleReferencingTypeObj::ExampleReferencingTypeObj() :
    ObjBase{{albers::ObjectID::untracked,albers::ObjectID::untracked},1},
    data()
    { }

ExampleReferencingTypeObj::ExampleReferencingTypeObj(const albers::ObjectID id, ExampleReferencingTypeData data) :
    ObjBase{id,-1},
    data(data)
    { }

ExampleReferencingTypeObj::ExampleReferencingTypeObj(const ExampleReferencingTypeObj& other) :
    ObjBase{{albers::ObjectID::untracked,albers::ObjectID::untracked},0},
    data()
    { }

ExampleReferencingTypeObj::~ExampleReferencingTypeObj() {
  if (id.index == albers::ObjectID::untracked) {
delete m_Clusters;
delete m_Refs;

  }
}
