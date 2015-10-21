// datamodel specific includes
#include "ExampleCluster.h"
#include "ExampleClusterConst.h"
#include "ExampleClusterObj.h"
#include "ExampleClusterData.h"
#include "ExampleClusterCollection.h"
#include <iostream>

ExampleCluster::ExampleCluster() : m_obj(new ExampleClusterObj()){
 m_obj->acquire();
};

ExampleCluster::ExampleCluster(double energy) : m_obj(new ExampleClusterObj()){
 m_obj->acquire();
   m_obj->data.energy = energy;
};

ExampleCluster::ExampleCluster(const ExampleCluster& other) : m_obj(other.m_obj) {
  m_obj->acquire();
}

ExampleCluster& ExampleCluster::operator=(const ExampleCluster& other) {
  if ( m_obj != nullptr) m_obj->release();
  m_obj = other.m_obj;
  return *this;
}

ExampleCluster::ExampleCluster(ExampleClusterObj* obj) : m_obj(obj){
  if(m_obj != nullptr)
    m_obj->acquire();
}

ExampleCluster ExampleCluster::clone() const {
  return {new ExampleClusterObj(*m_obj)};
}

ExampleCluster::~ExampleCluster(){
  if ( m_obj != nullptr) m_obj->release();
}

ExampleCluster::operator ConstExampleCluster() const {return ConstExampleCluster(m_obj);};



std::vector<ConstExampleHit>::const_iterator ExampleCluster::Hits_begin() const {
  auto ret_value = m_obj->m_Hits->begin();
  std::advance(ret_value, m_obj->data.Hits_begin);
  return ret_value;
}

std::vector<ConstExampleHit>::const_iterator ExampleCluster::Hits_end() const {
  auto ret_value = m_obj->m_Hits->begin();
  std::advance(ret_value, m_obj->data.Hits_end-1);
  return ++ret_value;
}

void ExampleCluster::addHits(ConstExampleHit component) {
  m_obj->m_Hits->push_back(component);
  m_obj->data.Hits_end++;
}

unsigned int ExampleCluster::Hits_size() const {
  return (m_obj->data.Hits_end-m_obj->data.Hits_begin);
}

ConstExampleHit ExampleCluster::Hits(unsigned int index) const {
  if (Hits_size() > index) {
    return m_obj->m_Hits->at(m_obj->data.Hits_begin+index);
}
  else throw std::out_of_range ("index out of bounds for existing references");
}

bool  ExampleCluster::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const podio::ObjectID ExampleCluster::getObjectID() const {
  if (m_obj !=nullptr){
    return m_obj->id;
  }
  return podio::ObjectID{-2,-2};
}

bool ExampleCluster::operator==(const ConstExampleCluster& other) const {
     return (m_obj==other.m_obj);
}


//bool operator< (const ExampleCluster& p1, const ExampleCluster& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}
