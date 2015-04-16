// datamodel specific includes
#include "ExampleClusterObj.h"
#include "ExampleClusterData.h"
#include "ExampleClusterCollection.h"
#include <iostream>

ExampleCluster::ExampleCluster() : m_obj(new ExampleClusterObj()){};

ExampleCluster::ExampleCluster(const ExampleCluster& other) : m_obj(other.m_obj) {
  m_obj->increaseRefCount();
}


std::vector<ExampleHit>::const_iterator ExampleCluster::Hits_begin() const {
  auto ret_value = m_obj->m_Hits->begin();
  std::advance(ret_value, m_obj->data.Hits_begin);
  return ret_value;
}

std::vector<ExampleHit>::const_iterator ExampleCluster::Hits_end() const {
  auto ret_value = m_obj->m_Hits->begin();
  std::advance(ret_value, m_obj->data.Hits_end-1);
  return ++ret_value;
}

void ExampleCluster::addHits(ExampleHit& component) {
  m_obj->m_Hits->push_back(component);
  m_obj->data.Hits_end++;
}

bool  ExampleCluster::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const albers::ObjectID ExampleCluster::getObjectID() const {
  return m_obj->id;
}


ExampleCluster::ExampleCluster(ExampleClusterObj* obj) : m_obj(obj){
  if(m_obj != nullptr)
    m_obj->increaseRefCount();
}

ExampleCluster& ExampleCluster::operator=(const ExampleCluster& other){
  if ( m_obj != nullptr && m_obj->decreaseRefCount()==0) {
    std::cout << "deleting free-floating ExampleCluster at " << m_obj << std::endl;
    delete m_obj;
  }
  m_obj = other.m_obj;
  return *this;
}

ExampleCluster::~ExampleCluster(){
  if ( m_obj != nullptr && m_obj->decreaseRefCount()==0 ){
    std::cout << "deleting free-floating ExampleCluster at " << m_obj << std::endl;
    delete m_obj;
   }
}

//bool operator< (const ExampleCluster& p1, const ExampleCluster& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}
