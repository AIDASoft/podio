// datamodel specific includes
#include "ExampleReferencingType.h"
#include "ExampleReferencingTypeConst.h"
#include "ExampleReferencingTypeObj.h"
#include "ExampleReferencingTypeData.h"
#include "ExampleReferencingTypeCollection.h"
#include <iostream>

ConstExampleReferencingType::ConstExampleReferencingType() : m_obj(new ExampleReferencingTypeObj()){
 m_obj->acquire();
};



ConstExampleReferencingType::ConstExampleReferencingType(const ConstExampleReferencingType& other) : m_obj(other.m_obj) {
  m_obj->acquire();
}

ConstExampleReferencingType& ConstExampleReferencingType::operator=(const ConstExampleReferencingType& other) {
  if ( m_obj != nullptr) m_obj->release();
  m_obj = other.m_obj;
  return *this;
}

ConstExampleReferencingType::ConstExampleReferencingType(ExampleReferencingTypeObj* obj) : m_obj(obj){
  if(m_obj != nullptr)
    m_obj->acquire();
}

ConstExampleReferencingType ConstExampleReferencingType::clone() const {
  return {new ExampleReferencingTypeObj(*m_obj)};
}

ConstExampleReferencingType::~ConstExampleReferencingType(){
  if ( m_obj != nullptr) m_obj->release();
}


std::vector<ConstExampleCluster>::const_iterator ConstExampleReferencingType::Clusters_begin() const {
  auto ret_value = m_obj->m_Clusters->begin();
  std::advance(ret_value, m_obj->data.Clusters_begin);
  return ret_value;
}

std::vector<ConstExampleCluster>::const_iterator ConstExampleReferencingType::Clusters_end() const {
  auto ret_value = m_obj->m_Clusters->begin();
  std::advance(ret_value, m_obj->data.Clusters_end-1);
  return ++ret_value;
}
std::vector<ConstExampleReferencingType>::const_iterator ConstExampleReferencingType::Refs_begin() const {
  auto ret_value = m_obj->m_Refs->begin();
  std::advance(ret_value, m_obj->data.Refs_begin);
  return ret_value;
}

std::vector<ConstExampleReferencingType>::const_iterator ConstExampleReferencingType::Refs_end() const {
  auto ret_value = m_obj->m_Refs->begin();
  std::advance(ret_value, m_obj->data.Refs_end-1);
  return ++ret_value;
}

bool  ConstExampleReferencingType::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const podio::ObjectID ConstExampleReferencingType::getObjectID() const {
  return m_obj->id;
}

bool ConstExampleReferencingType::operator==(const ExampleReferencingType& other) const {
     return (m_obj==other.m_obj);
}

//bool operator< (const ExampleReferencingType& p1, const ExampleReferencingType& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}
