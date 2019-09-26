// datamodel specific includes
#include "ExampleWithARelation.h"
#include "ExampleWithARelationConst.h"
#include "ExampleWithARelationObj.h"
#include "ExampleWithARelationData.h"
#include "ExampleWithARelationCollection.h"
#include <iostream>
#include "ExampleWithNamespace.h"


namespace ex42 {

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

ExampleWithARelation::operator ConstExampleWithARelation() const {return ConstExampleWithARelation(m_obj);}

  const float& ExampleWithARelation::number() const { return m_obj->data.number; }
  const ex42::ConstExampleWithNamespace ExampleWithARelation::ref() const {
    if (m_obj->m_ref == nullptr) {
      return ex42::ConstExampleWithNamespace(nullptr);
    }
    return ex42::ConstExampleWithNamespace(*(m_obj->m_ref));
  }
void ExampleWithARelation::number(float value) { m_obj->data.number = value; }
void ExampleWithARelation::ref(ex42::ConstExampleWithNamespace value) {
  if (m_obj->m_ref != nullptr) delete m_obj->m_ref;
  m_obj->m_ref = new ConstExampleWithNamespace(value);
}

std::vector<ex42::ConstExampleWithNamespace>::const_iterator ExampleWithARelation::refs_begin() const {
  auto ret_value = m_obj->m_refs->begin();
  std::advance(ret_value, m_obj->data.refs_begin);
  return ret_value;
}

std::vector<ex42::ConstExampleWithNamespace>::const_iterator ExampleWithARelation::refs_end() const {
  auto ret_value = m_obj->m_refs->begin();
  std::advance(ret_value, m_obj->data.refs_end);
  return ret_value;
}

void ExampleWithARelation::addrefs(ex42::ConstExampleWithNamespace component) {
  m_obj->m_refs->push_back(component);
  m_obj->data.refs_end++;
}

unsigned int ExampleWithARelation::refs_size() const {
  return (m_obj->data.refs_end-m_obj->data.refs_begin);
}

ex42::ConstExampleWithNamespace ExampleWithARelation::refs(unsigned int index) const {
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

bool ExampleWithARelation::operator==(const ConstExampleWithARelation& other) const {
  return (m_obj==other.m_obj);
}

std::ostream& operator<<( std::ostream& o,const ConstExampleWithARelation& value ){
  o << " id : " << value.id() << std::endl ;
  o << " number : " << value.number() << std::endl ;
  o << " ref : " << value.ref().id() << std::endl ;
  o << " refs : " ;
  for(unsigned i=0,N=value.refs_size(); i<N ; ++i)
    o << value.refs(i) << " " ; 
  o << std::endl ;
  return o ;
}


//bool operator< (const ExampleWithARelation& p1, const ExampleWithARelation& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}

} // namespace ex42
