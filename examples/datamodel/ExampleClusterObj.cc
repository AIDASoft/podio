#include "ExampleClusterObj.h"

ExampleClusterObj::ExampleClusterObj() :
    ObjBase{{podio::ObjectID::untracked,podio::ObjectID::untracked},0}
    ,data()
    ,m_Hits(new std::vector<ExampleHit>())
    { }

ExampleClusterObj::ExampleClusterObj(const podio::ObjectID id, ExampleClusterData data) :
    ObjBase{id,0},
    data(data)
    { }

ExampleClusterObj::ExampleClusterObj(const ExampleClusterObj& other) :
    ObjBase{{podio::ObjectID::untracked,podio::ObjectID::untracked},0}
    ,data(other.data)
    ,m_Hits(new std::vector<ExampleHit>(*(other.m_Hits)))
    { }

ExampleClusterObj::~ExampleClusterObj() {
  if (id.index == podio::ObjectID::untracked) {
delete m_Hits;

  }
}
