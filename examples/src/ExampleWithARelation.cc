// datamodel specific includes
#include "ExampleWithARelation.h"
#include "ExampleWithARelationConst.h"
#include "ExampleWithARelationObj.h"
#include "ExampleWithARelationData.h"
#include "ExampleWithARelationCollection.h"
#include <iostream>
#include "ExampleWithNamespace.h"


namespace ex {

ExampleWithARelation::ExampleWithARelation() : m_obj(new ExampleWithARelationObj()){
 m_obj->acquire();
}



ExampleWithARelation::ExampleWithARelation(const ExampleWithARelation& other) : m_obj(other.m_obj) {
  m_obj->acquire();
}

ExampleWithARelation& ExampleWithARelation::operator=(const ExampleWithARelation& other) {
  if ( m_obj != nullptr) m_obj->release();
  m_obj = other.m_obj;
  return *this;
}

ExampleWithARelation::ExampleWithARelation(ExampleWithARelationObj* obj) : m_obj(obj){
  if(m_obj != nullptr)
    m_obj->acquire();
}

ExampleWithARelation ExampleWithARelation::clone() const {
  return {new ExampleWithARelationObj(*m_obj)};
}

ExampleWithARelation::~ExampleWithARelation(){
  if ( m_obj != nullptr) m_obj->release();
}

ExampleWithARelation::operator ConstExampleWithARelation() const {return ConstExampleWithARelation(m_obj);}

  const ex::ConstExampleWithNamespace ExampleWithARelation::ref() const { if (m_obj->m_ref == nullptr) {
 return ex::ConstExampleWithNamespace(nullptr);}
 return ex::ConstExampleWithNamespace(*(m_obj->m_ref));}

void ExampleWithARelation::ref(ex::ConstExampleWithNamespace value) { if (m_obj->m_ref != nullptr) delete m_obj->m_ref; m_obj->m_ref = new ConstExampleWithNamespace(value); }


bool  ExampleWithARelation::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const podio::ObjectID ExampleWithARelation::getObjectID() const {
  if (m_obj !=nullptr){
    return m_obj->id;
  }
  return podio::ObjectID{-2,-2};
}

bool ExampleWithARelation::operator==(const ConstExampleWithARelation& other) const {
     return (m_obj==other.m_obj);
}


//bool operator< (const ExampleWithARelation& p1, const ExampleWithARelation& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}

} // namespace ex
