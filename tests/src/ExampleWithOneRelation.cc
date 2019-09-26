// datamodel specific includes
#include "ExampleWithOneRelation.h"
#include "ExampleWithOneRelationConst.h"
#include "ExampleWithOneRelationObj.h"
#include "ExampleWithOneRelationData.h"
#include "ExampleWithOneRelationCollection.h"
#include <iostream>
#include "ExampleCluster.h"




ExampleWithOneRelation::ExampleWithOneRelation() : m_obj(new ExampleWithOneRelationObj()){
 m_obj->acquire();
}



ExampleWithOneRelation::ExampleWithOneRelation(const ExampleWithOneRelation& other) : m_obj(other.m_obj) {
  m_obj->acquire();
}

ExampleWithOneRelation& ExampleWithOneRelation::operator=(const ExampleWithOneRelation& other) {
  if ( m_obj != nullptr) m_obj->release();
  m_obj = other.m_obj;
  return *this;
}

ExampleWithOneRelation::ExampleWithOneRelation(ExampleWithOneRelationObj* obj) : m_obj(obj){
  if(m_obj != nullptr)
    m_obj->acquire();
}

ExampleWithOneRelation ExampleWithOneRelation::clone() const {
  return {new ExampleWithOneRelationObj(*m_obj)};
}

ExampleWithOneRelation::~ExampleWithOneRelation(){
  if ( m_obj != nullptr) m_obj->release();
}

ExampleWithOneRelation::operator ConstExampleWithOneRelation() const {return ConstExampleWithOneRelation(m_obj);}

  const ::ConstExampleCluster ExampleWithOneRelation::cluster() const {
    if (m_obj->m_cluster == nullptr) {
      return ::ConstExampleCluster(nullptr);
    }
    return ::ConstExampleCluster(*(m_obj->m_cluster));
  }
void ExampleWithOneRelation::cluster(::ConstExampleCluster value) {
  if (m_obj->m_cluster != nullptr) delete m_obj->m_cluster;
  m_obj->m_cluster = new ConstExampleCluster(value);
}



bool  ExampleWithOneRelation::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const podio::ObjectID ExampleWithOneRelation::getObjectID() const {
  if (m_obj !=nullptr){
    return m_obj->id;
  }
  return podio::ObjectID{-2,-2};
}

bool ExampleWithOneRelation::operator==(const ConstExampleWithOneRelation& other) const {
  return (m_obj==other.m_obj);
}

std::ostream& operator<<( std::ostream& o,const ConstExampleWithOneRelation& value ){
  o << " id : " << value.id() << std::endl ;
  o << " cluster : " << value.cluster().id() << std::endl ;
  return o ;
}


//bool operator< (const ExampleWithOneRelation& p1, const ExampleWithOneRelation& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}


