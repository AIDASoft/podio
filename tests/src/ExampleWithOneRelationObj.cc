#include "ExampleWithOneRelationObj.h"
#include "ExampleClusterConst.h"

ExampleWithOneRelationObj::ExampleWithOneRelationObj()
    : ObjBase{{podio::ObjectID::untracked, podio::ObjectID::untracked}, 0},
      data(), m_cluster(nullptr)

{}

ExampleWithOneRelationObj::ExampleWithOneRelationObj(
    const podio::ObjectID id, ExampleWithOneRelationData data)
    : ObjBase{id, 0}, data(data) {}

ExampleWithOneRelationObj::ExampleWithOneRelationObj(
    const ExampleWithOneRelationObj &other)
    : ObjBase{{podio::ObjectID::untracked, podio::ObjectID::untracked}, 0},
      data(other.data), m_cluster(nullptr) {
  if (other.m_cluster != nullptr) {
    m_cluster = new ConstExampleCluster(*(other.m_cluster));
  }
}

ExampleWithOneRelationObj::~ExampleWithOneRelationObj() {
  if (id.index == podio::ObjectID::untracked) {
  }
  if (m_cluster != nullptr)
    delete m_cluster;
}
