// datamodel specific includes
#include "ExampleWithComponent.h"
#include "ExampleWithComponentConst.h"
#include "ExampleWithComponentObj.h"
#include "ExampleWithComponentData.h"
#include "ExampleWithComponentCollection.h"
#include <iostream>




ExampleWithComponent::ExampleWithComponent() : m_obj(new ExampleWithComponentObj()){
 m_obj->acquire();
}

ExampleWithComponent::ExampleWithComponent(NotSoSimpleStruct component) : m_obj(new ExampleWithComponentObj()) {
  m_obj->acquire();
    m_obj->data.component = component;
}


ExampleWithComponent::ExampleWithComponent(const ExampleWithComponent& other) : m_obj(other.m_obj) {
  m_obj->acquire();
}

ExampleWithComponent& ExampleWithComponent::operator=(const ExampleWithComponent& other) {
  if ( m_obj != nullptr) m_obj->release();
  m_obj = other.m_obj;
  return *this;
}

ExampleWithComponent::ExampleWithComponent(ExampleWithComponentObj* obj) : m_obj(obj){
  if(m_obj != nullptr)
    m_obj->acquire();
}

ExampleWithComponent ExampleWithComponent::clone() const {
  return {new ExampleWithComponentObj(*m_obj)};
}

ExampleWithComponent::~ExampleWithComponent(){
  if ( m_obj != nullptr) m_obj->release();
}

ExampleWithComponent::operator ConstExampleWithComponent() const {return ConstExampleWithComponent(m_obj);}

  const NotSoSimpleStruct& ExampleWithComponent::component() const { return m_obj->data.component; }

  NotSoSimpleStruct& ExampleWithComponent::component() { return m_obj->data.component; }
void ExampleWithComponent::component(class NotSoSimpleStruct value) { m_obj->data.component = value; }



bool  ExampleWithComponent::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const podio::ObjectID ExampleWithComponent::getObjectID() const {
  if (m_obj !=nullptr){
    return m_obj->id;
  }
  return podio::ObjectID{-2,-2};
}

bool ExampleWithComponent::operator==(const ConstExampleWithComponent& other) const {
  return (m_obj==other.m_obj);
}


//bool operator< (const ExampleWithComponent& p1, const ExampleWithComponent& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}


