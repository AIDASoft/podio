// datamodel specific includes
#include "ExampleWithComponent.h"
#include "ExampleWithComponentConst.h"
#include "ExampleWithComponentObj.h"
#include "ExampleWithComponentData.h"
#include "ExampleWithComponentCollection.h"
#include <iostream>




ConstExampleWithComponent::ConstExampleWithComponent() : m_obj(new ExampleWithComponentObj()) {
 m_obj->acquire();
}

ConstExampleWithComponent::ConstExampleWithComponent(NotSoSimpleStruct component) : m_obj(new ExampleWithComponentObj()){
 m_obj->acquire();
   m_obj->data.component = component;
}


ConstExampleWithComponent::ConstExampleWithComponent(const ConstExampleWithComponent& other) : m_obj(other.m_obj) {
  m_obj->acquire();
}

ConstExampleWithComponent& ConstExampleWithComponent::operator=(const ConstExampleWithComponent& other) {
  if ( m_obj != nullptr) m_obj->release();
  m_obj = other.m_obj;
  return *this;
}

ConstExampleWithComponent::ConstExampleWithComponent(ExampleWithComponentObj* obj) : m_obj(obj) {
  if(m_obj != nullptr)
    m_obj->acquire();
}

ConstExampleWithComponent ConstExampleWithComponent::clone() const {
  return {new ExampleWithComponentObj(*m_obj)};
}

ConstExampleWithComponent::~ConstExampleWithComponent(){
  if ( m_obj != nullptr) m_obj->release();
}

  const SimpleStruct& ConstExampleWithComponent::data() const { return m_obj->data.component.data; }
  /// Access the  a component
  const NotSoSimpleStruct& ConstExampleWithComponent::component() const { return m_obj->data.component; }



bool  ConstExampleWithComponent::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const podio::ObjectID ConstExampleWithComponent::getObjectID() const {
  if (m_obj !=nullptr){
    return m_obj->id;
  }
  return podio::ObjectID{-2,-2};
}

bool ConstExampleWithComponent::operator==(const ExampleWithComponent& other) const {
     return (m_obj==other.m_obj);
}

//bool operator< (const ExampleWithComponent& p1, const ExampleWithComponent& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}


