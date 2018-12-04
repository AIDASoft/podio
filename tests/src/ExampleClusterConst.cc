// datamodel specific includes
#include "ExampleClusterConst.h"
#include "ExampleCluster.h"
#include "ExampleClusterCollection.h"
#include "ExampleClusterData.h"
#include "ExampleClusterObj.h"
#include <iostream>

ConstExampleCluster::ConstExampleCluster() : m_obj(new ExampleClusterObj()) {
  m_obj->acquire();
}

ConstExampleCluster::ConstExampleCluster(double energy)
    : m_obj(new ExampleClusterObj()) {
  m_obj->acquire();
  m_obj->data.energy = energy;
}

ConstExampleCluster::ConstExampleCluster(const ConstExampleCluster &other)
    : m_obj(other.m_obj) {
  m_obj->acquire();
}

ConstExampleCluster &ConstExampleCluster::
operator=(const ConstExampleCluster &other) {
  if (m_obj != nullptr)
    m_obj->release();
  m_obj = other.m_obj;
  return *this;
}

ConstExampleCluster::ConstExampleCluster(ExampleClusterObj *obj) : m_obj(obj) {
  if (m_obj != nullptr)
    m_obj->acquire();
}

ConstExampleCluster ConstExampleCluster::clone() const {
  return {new ExampleClusterObj(*m_obj)};
}

ConstExampleCluster::~ConstExampleCluster() {
  if (m_obj != nullptr)
    m_obj->release();
}

/// Access the  cluster energy
const double &ConstExampleCluster::energy() const { return m_obj->data.energy; }

std::vector<::ConstExampleHit>::const_iterator
ConstExampleCluster::Hits_begin() const {
  auto ret_value = m_obj->m_Hits->begin();
  std::advance(ret_value, m_obj->data.Hits_begin);
  return ret_value;
}

std::vector<::ConstExampleHit>::const_iterator
ConstExampleCluster::Hits_end() const {
  auto ret_value = m_obj->m_Hits->begin();
  std::advance(ret_value, m_obj->data.Hits_end - 1);
  return ++ret_value;
}

unsigned int ConstExampleCluster::Hits_size() const {
  return (m_obj->data.Hits_end - m_obj->data.Hits_begin);
}

::ConstExampleHit ConstExampleCluster::Hits(unsigned int index) const {
  if (Hits_size() > index) {
    return m_obj->m_Hits->at(m_obj->data.Hits_begin + index);
  } else
    throw std::out_of_range("index out of bounds for existing references");
}
std::vector<::ConstExampleCluster>::const_iterator
ConstExampleCluster::Clusters_begin() const {
  auto ret_value = m_obj->m_Clusters->begin();
  std::advance(ret_value, m_obj->data.Clusters_begin);
  return ret_value;
}

std::vector<::ConstExampleCluster>::const_iterator
ConstExampleCluster::Clusters_end() const {
  auto ret_value = m_obj->m_Clusters->begin();
  std::advance(ret_value, m_obj->data.Clusters_end - 1);
  return ++ret_value;
}

unsigned int ConstExampleCluster::Clusters_size() const {
  return (m_obj->data.Clusters_end - m_obj->data.Clusters_begin);
}

::ConstExampleCluster ConstExampleCluster::Clusters(unsigned int index) const {
  if (Clusters_size() > index) {
    return m_obj->m_Clusters->at(m_obj->data.Clusters_begin + index);
  } else
    throw std::out_of_range("index out of bounds for existing references");
}

bool ConstExampleCluster::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const podio::ObjectID ConstExampleCluster::getObjectID() const {
  if (m_obj != nullptr) {
    return m_obj->id;
  }
  return podio::ObjectID{-2, -2};
}

bool ConstExampleCluster::operator==(const ExampleCluster &other) const {
  return (m_obj == other.m_obj);
}

// bool operator< (const ExampleCluster& p1, const ExampleCluster& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}
