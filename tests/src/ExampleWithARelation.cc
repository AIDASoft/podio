// datamodel specific includes
#include "ExampleWithARelation.h"
#include "ExampleWithARelationObj.h"
#include "ExampleWithARelationData.h"
#include "ExampleWithARelationCollection.h"
#include <iostream>
#include "ExampleWithNamespace.h"


namespace ex {

ExampleWithARelation::ExampleWithARelation() : m_obj(new ExampleWithARelationObj()){
 m_obj->acquire();
}

ExampleWithARelation::ExampleWithARelation(float number) : m_obj(new ExampleWithARelationObj()) {
  m_obj->acquire();
    m_obj->data.number = number;
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

  const float& ExampleWithARelation::number() const { return m_obj->data.number; }
  const ex::ExampleWithNamespace ExampleWithARelation::ref() const {
    if (m_obj->m_ref == nullptr) {
      return ex::ExampleWithNamespace(nullptr);
    }
    return ex::ExampleWithNamespace(*(m_obj->m_ref));
  }
void ExampleWithARelation::number(float value){ m_obj->checkAccess(); m_obj->data.number = value; }
void ExampleWithARelation::ref(ex::ExampleWithNamespace value) {
  if (m_obj->m_ref != nullptr) delete m_obj->m_ref;
  m_obj->m_ref = new ExampleWithNamespace(value);
}

std::vector<ex::ExampleWithNamespace>::const_iterator ExampleWithARelation::refs_begin() const {
  auto ret_value = m_obj->m_refs->begin();
  std::advance(ret_value, m_obj->data.refs_begin);
  return ret_value;
}

std::vector<ex::ExampleWithNamespace>::const_iterator ExampleWithARelation::refs_end() const {
  auto ret_value = m_obj->m_refs->begin();
//fg: this code fails if m_obj->data.refs==0
//  std::advance(ret_value, m_obj->data.refs_end-1);
//  return ++ret_value;
  std::advance(ret_value, m_obj->data.refs_end);
  return ret_value;
}

void ExampleWithARelation::addrefs(ex::ExampleWithNamespace component) {
  m_obj->m_refs->push_back(component);
  m_obj->data.refs_end++;
}

unsigned int ExampleWithARelation::refs_size() const {
  return (m_obj->data.refs_end-m_obj->data.refs_begin);
}

ex::ExampleWithNamespace ExampleWithARelation::refs(unsigned int index) const {
  if (refs_size() > index) {
    return m_obj->m_refs->at(m_obj->data.refs_begin+index);
  }
  else throw std::out_of_range ("index out of bounds for existing references");
}


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


//bool operator< (const ExampleWithARelation& p1, const ExampleWithARelation& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}

} // namespace ex
