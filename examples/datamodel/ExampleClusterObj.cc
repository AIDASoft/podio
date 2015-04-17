#include "ExampleClusterObj.h"

ExampleClusterObj::ExampleClusterObj() :
    ObjBase{{albers::ObjectID::untracked,albers::ObjectID::untracked},0}
    ,data()
    ,m_Hits(new std::vector<ExampleHit>())
    { }

ExampleClusterObj::ExampleClusterObj(const albers::ObjectID id, ExampleClusterData data) :
    ObjBase{id,0},
    data(data)
    { }

ExampleClusterObj::ExampleClusterObj(const ExampleClusterObj& other) :
    ObjBase{{albers::ObjectID::untracked,albers::ObjectID::untracked},0}
    ,data(other.data)
    ,m_Hits(new std::vector<ExampleHit>(*(other.m_Hits)))
    { }

ExampleClusterObj::~ExampleClusterObj() {
  if (id.index == albers::ObjectID::untracked) {
delete m_Hits;

  }
}
