#include "ExampleWithOneRelationObj.h"
#include "ExampleClusterConst.h"


ExampleWithOneRelationObj::ExampleWithOneRelationObj() :
    ObjBase{{podio::ObjectID::untracked,podio::ObjectID::untracked},0}
    ,data()
    ,m_cluster(new ConstExampleCluster())

    { }

ExampleWithOneRelationObj::ExampleWithOneRelationObj(const podio::ObjectID id, ExampleWithOneRelationData data) :
    ObjBase{id,0},
    data(data)
    { }

ExampleWithOneRelationObj::ExampleWithOneRelationObj(const ExampleWithOneRelationObj& other) :
    ObjBase{{podio::ObjectID::untracked,podio::ObjectID::untracked},0}
    ,data(other.data)
    
    { }

ExampleWithOneRelationObj::~ExampleWithOneRelationObj() {
  if (id.index == podio::ObjectID::untracked) {
delete m_cluster;

  }
}
