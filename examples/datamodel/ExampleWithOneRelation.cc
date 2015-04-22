// datamodel specific includes
#include "ExampleWithOneRelationObj.h"
#include "ExampleWithOneRelationData.h"
#include "ExampleWithOneRelationCollection.h"
#include <iostream>

ExampleWithOneRelation::ExampleWithOneRelation() : m_obj(new ExampleWithOneRelationObj()){
 m_obj->acquire();
};



ExampleWithOneRelation::ExampleWithOneRelation(const ExampleWithOneRelation& other) : m_obj(other.m_obj) {
  m_obj->acquire();
}

ExampleWithOneRelation& ExampleWithOneRelation::operator=(const ExampleWithOneRelation& other){
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



bool  ExampleWithOneRelation::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const albers::ObjectID ExampleWithOneRelation::getObjectID() const {
  return m_obj->id;
}


//bool operator< (const ExampleWithOneRelation& p1, const ExampleWithOneRelation& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}
