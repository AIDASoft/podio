// datamodel specific includes
#include "ExampleReferencingType.h"
#include "ExampleReferencingTypeConst.h"
#include "ExampleReferencingTypeObj.h"
#include "ExampleReferencingTypeData.h"
#include "ExampleReferencingTypeCollection.h"
#include <iostream>




ExampleReferencingType::ExampleReferencingType() : m_obj(new ExampleReferencingTypeObj()){
 m_obj->acquire();
}



ExampleReferencingType::ExampleReferencingType(const ExampleReferencingType& other) : m_obj(other.m_obj) {
  m_obj->acquire();
}

ExampleReferencingType& ExampleReferencingType::operator=(const ExampleReferencingType& other) {
  if ( m_obj != nullptr) m_obj->release();
  m_obj = other.m_obj;
  return *this;
}

ExampleReferencingType::ExampleReferencingType(ExampleReferencingTypeObj* obj) : m_obj(obj){
  if(m_obj != nullptr)
    m_obj->acquire();
}

ExampleReferencingType ExampleReferencingType::clone() const {
  return {new ExampleReferencingTypeObj(*m_obj)};
}

ExampleReferencingType::~ExampleReferencingType(){
  if ( m_obj != nullptr) m_obj->release();
}

ExampleReferencingType::operator ConstExampleReferencingType() const {return ConstExampleReferencingType(m_obj);}



std::vector<::ConstExampleCluster>::const_iterator ExampleReferencingType::Clusters_begin() const {
  auto ret_value = m_obj->m_Clusters->begin();
  std::advance(ret_value, m_obj->data.Clusters_begin);
  return ret_value;
}

std::vector<::ConstExampleCluster>::const_iterator ExampleReferencingType::Clusters_end() const {
  auto ret_value = m_obj->m_Clusters->begin();
  std::advance(ret_value, m_obj->data.Clusters_end);
  return ret_value;
}

void ExampleReferencingType::addClusters(::ConstExampleCluster component) {
  m_obj->m_Clusters->push_back(component);
  m_obj->data.Clusters_end++;
}

unsigned int ExampleReferencingType::Clusters_size() const {
  return (m_obj->data.Clusters_end-m_obj->data.Clusters_begin);
}

::ConstExampleCluster ExampleReferencingType::Clusters(unsigned int index) const {
  if (Clusters_size() > index) {
    return m_obj->m_Clusters->at(m_obj->data.Clusters_begin+index);
  }
  else throw std::out_of_range ("index out of bounds for existing references");
}
std::vector<::ConstExampleReferencingType>::const_iterator ExampleReferencingType::Refs_begin() const {
  auto ret_value = m_obj->m_Refs->begin();
  std::advance(ret_value, m_obj->data.Refs_begin);
  return ret_value;
}

std::vector<::ConstExampleReferencingType>::const_iterator ExampleReferencingType::Refs_end() const {
  auto ret_value = m_obj->m_Refs->begin();
  std::advance(ret_value, m_obj->data.Refs_end);
  return ret_value;
}

void ExampleReferencingType::addRefs(::ConstExampleReferencingType component) {
  m_obj->m_Refs->push_back(component);
  m_obj->data.Refs_end++;
}

unsigned int ExampleReferencingType::Refs_size() const {
  return (m_obj->data.Refs_end-m_obj->data.Refs_begin);
}

::ConstExampleReferencingType ExampleReferencingType::Refs(unsigned int index) const {
  if (Refs_size() > index) {
    return m_obj->m_Refs->at(m_obj->data.Refs_begin+index);
  }
  else throw std::out_of_range ("index out of bounds for existing references");
}


bool  ExampleReferencingType::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const podio::ObjectID ExampleReferencingType::getObjectID() const {
  if (m_obj !=nullptr){
    return m_obj->id;
  }
  return podio::ObjectID{-2,-2};
}

bool ExampleReferencingType::operator==(const ConstExampleReferencingType& other) const {
  return (m_obj==other.m_obj);
}

std::ostream& operator<<( std::ostream& o,const ConstExampleReferencingType& value ){
  o << " id : " << value.id() << std::endl ;
  o << " Clusters : " ;
  for(unsigned i=0,N=value.Clusters_size(); i<N ; ++i)
    o << value.Clusters(i) << " " ; 
  o << std::endl ;
  o << " Refs : " ;
  for(unsigned i=0,N=value.Refs_size(); i<N ; ++i)
    o << value.Refs(i) << " " ; 
  o << std::endl ;
  return o ;
}


//bool operator< (const ExampleReferencingType& p1, const ExampleReferencingType& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}


