// datamodel specific includes
#include "ExampleWithOneRelationConst.h"
#include "ExampleCluster.h"
#include "ExampleWithOneRelation.h"
#include "ExampleWithOneRelationCollection.h"
#include "ExampleWithOneRelationData.h"
#include "ExampleWithOneRelationObj.h"
#include <iostream>

ConstExampleWithOneRelation::ConstExampleWithOneRelation()
    : m_obj(new ExampleWithOneRelationObj()) {
  m_obj->acquire();
}

ConstExampleWithOneRelation::ConstExampleWithOneRelation(
    const ConstExampleWithOneRelation &other)
    : m_obj(other.m_obj) {
  m_obj->acquire();
}

ConstExampleWithOneRelation &ConstExampleWithOneRelation::
operator=(const ConstExampleWithOneRelation &other) {
  if (m_obj != nullptr)
    m_obj->release();
  m_obj = other.m_obj;
  return *this;
}

ConstExampleWithOneRelation::ConstExampleWithOneRelation(
    ExampleWithOneRelationObj *obj)
    : m_obj(obj) {
  if (m_obj != nullptr)
    m_obj->acquire();
}

ConstExampleWithOneRelation ConstExampleWithOneRelation::clone() const {
  return {new ExampleWithOneRelationObj(*m_obj)};
}

ConstExampleWithOneRelation::~ConstExampleWithOneRelation() {
  if (m_obj != nullptr)
    m_obj->release();
}

/// Access the  a particular cluster
const ::ConstExampleCluster ConstExampleWithOneRelation::cluster() const {
  if (m_obj->m_cluster == nullptr) {
    return ::ConstExampleCluster(nullptr);
  }
  return ::ConstExampleCluster(*(m_obj->m_cluster));
}

bool ConstExampleWithOneRelation::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const podio::ObjectID ConstExampleWithOneRelation::getObjectID() const {
  if (m_obj != nullptr) {
    return m_obj->id;
  }
  return podio::ObjectID{-2, -2};
}

bool ConstExampleWithOneRelation::
operator==(const ExampleWithOneRelation &other) const {
  return (m_obj == other.m_obj);
}

// bool operator< (const ExampleWithOneRelation& p1, const
// ExampleWithOneRelation& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}
