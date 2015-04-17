#include "ExampleReferencingTypeObj.h"

ExampleReferencingTypeObj::ExampleReferencingTypeObj() :
    ObjBase{{albers::ObjectID::untracked,albers::ObjectID::untracked},0}
    ,data()
    ,m_Clusters(new std::vector<ExampleCluster>()),m_Refs(new std::vector<ExampleReferencingType>())
    { }

ExampleReferencingTypeObj::ExampleReferencingTypeObj(const albers::ObjectID id, ExampleReferencingTypeData data) :
    ObjBase{id,0},
    data(data)
    { }

ExampleReferencingTypeObj::ExampleReferencingTypeObj(const ExampleReferencingTypeObj& other) :
    ObjBase{{albers::ObjectID::untracked,albers::ObjectID::untracked},0}
    ,data(other.data)
    ,m_Clusters(new std::vector<ExampleCluster>(*(other.m_Clusters))),m_Refs(new std::vector<ExampleReferencingType>(*(other.m_Refs)))
    { }

ExampleReferencingTypeObj::~ExampleReferencingTypeObj() {
  if (id.index == albers::ObjectID::untracked) {
delete m_Clusters;
delete m_Refs;

  }
}
