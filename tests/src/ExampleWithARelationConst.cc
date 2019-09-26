// datamodel specific includes
#include "ExampleWithARelation.h"
#include "ExampleWithARelationConst.h"
#include "ExampleWithARelationObj.h"
#include "ExampleWithARelationData.h"
#include "ExampleWithARelationCollection.h"
#include <iostream>
#include "ExampleWithNamespace.h"


namespace ex42 {

ConstExampleWithARelation::ConstExampleWithARelation() : m_obj(new ExampleWithARelationObj()) {
 m_obj->acquire();
}

ConstExampleWithARelation::ConstExampleWithARelation(float number) : m_obj(new ExampleWithARelationObj()){
 m_obj->acquire();
   m_obj->data.number = number;
}


ConstExampleWithARelation::ConstExampleWithARelation(const ConstExampleWithARelation& other) : m_obj(other.m_obj) {
  m_obj->acquire();
}

ConstExampleWithARelation& ConstExampleWithARelation::operator=(const ConstExampleWithARelation& other) {
  if ( m_obj != nullptr) m_obj->release();
  m_obj = other.m_obj;
  return *this;
}

ConstExampleWithARelation::ConstExampleWithARelation(ExampleWithARelationObj* obj) : m_obj(obj) {
  if(m_obj != nullptr)
    m_obj->acquire();
}

ConstExampleWithARelation ConstExampleWithARelation::clone() const {
  return {new ExampleWithARelationObj(*m_obj)};
}

ConstExampleWithARelation::~ConstExampleWithARelation(){
  if ( m_obj != nullptr) m_obj->release();
}

  /// Access the  just a number
  const float& ConstExampleWithARelation::number() const { return m_obj->data.number; }
  /// Access the  a ref in a namespace
  const ex42::ConstExampleWithNamespace ConstExampleWithARelation::ref() const {
    if (m_obj->m_ref == nullptr) {
      return ex42::ConstExampleWithNamespace(nullptr);
    }
    return ex42::ConstExampleWithNamespace(*(m_obj->m_ref));}
std::vector<ex42::ConstExampleWithNamespace>::const_iterator ConstExampleWithARelation::refs_begin() const {
  auto ret_value = m_obj->m_refs->begin();
  std::advance(ret_value, m_obj->data.refs_begin);
  return ret_value;
}

std::vector<ex42::ConstExampleWithNamespace>::const_iterator ConstExampleWithARelation::refs_end() const {
  auto ret_value = m_obj->m_refs->begin();
  std::advance(ret_value, m_obj->data.refs_end-1);
  return ++ret_value;
}

unsigned int ConstExampleWithARelation::refs_size() const {
  return (m_obj->data.refs_end-m_obj->data.refs_begin);
}

ex42::ConstExampleWithNamespace ConstExampleWithARelation::refs(unsigned int index) const {
  if (refs_size() > index) {
    return m_obj->m_refs->at(m_obj->data.refs_begin+index);
  }
  else throw std::out_of_range ("index out of bounds for existing references");
}


bool  ConstExampleWithARelation::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const podio::ObjectID ConstExampleWithARelation::getObjectID() const {
  if (m_obj !=nullptr){
    return m_obj->id;
  }
  return podio::ObjectID{-2,-2};
}

bool ConstExampleWithARelation::operator==(const ExampleWithARelation& other) const {
     return (m_obj==other.m_obj);
}

//bool operator< (const ExampleWithARelation& p1, const ExampleWithARelation& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}

} // namespace ex42
