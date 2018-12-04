// datamodel specific includes
#include "ExampleWithComponent.h"
#include "ExampleWithComponentCollection.h"
#include "ExampleWithComponentConst.h"
#include "ExampleWithComponentData.h"
#include "ExampleWithComponentObj.h"
#include <iostream>

ExampleWithComponent::ExampleWithComponent()
    : m_obj(new ExampleWithComponentObj()) {
  m_obj->acquire();
}

ExampleWithComponent::ExampleWithComponent(NotSoSimpleStruct component)
    : m_obj(new ExampleWithComponentObj()) {
  m_obj->acquire();
  m_obj->data.component = component;
}

ExampleWithComponent::ExampleWithComponent(const ExampleWithComponent &other)
    : m_obj(other.m_obj) {
  m_obj->acquire();
}

ExampleWithComponent &ExampleWithComponent::
operator=(const ExampleWithComponent &other) {
  if (m_obj != nullptr)
    m_obj->release();
  m_obj = other.m_obj;
  return *this;
}

ExampleWithComponent::ExampleWithComponent(ExampleWithComponentObj *obj)
    : m_obj(obj) {
  if (m_obj != nullptr)
    m_obj->acquire();
}

ExampleWithComponent ExampleWithComponent::clone() const {
  return {new ExampleWithComponentObj(*m_obj)};
}

ExampleWithComponent::~ExampleWithComponent() {
  if (m_obj != nullptr)
    m_obj->release();
}

ExampleWithComponent::operator ConstExampleWithComponent() const {
  return ConstExampleWithComponent(m_obj);
}

const NotSoSimpleStruct &ExampleWithComponent::component() const {
  return m_obj->data.component;
}
const SimpleStruct &ExampleWithComponent::data() const {
  return m_obj->data.component.data;
}

NotSoSimpleStruct &ExampleWithComponent::component() {
  return m_obj->data.component;
}
void ExampleWithComponent::component(class NotSoSimpleStruct value) {
  m_obj->data.component = value;
}
SimpleStruct &ExampleWithComponent::data() {
  return m_obj->data.component.data;
}
void ExampleWithComponent::data(class SimpleStruct value) {
  m_obj->data.component.data = value;
}

bool ExampleWithComponent::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const podio::ObjectID ExampleWithComponent::getObjectID() const {
  if (m_obj != nullptr) {
    return m_obj->id;
  }
  return podio::ObjectID{-2, -2};
}

bool ExampleWithComponent::
operator==(const ConstExampleWithComponent &other) const {
  return (m_obj == other.m_obj);
}

std::ostream &operator<<(std::ostream &o,
                         const ConstExampleWithComponent &value) {
  o << " id : " << value.id() << std::endl;
  o << " component : " << value.component() << std::endl;
  o << " data : " << value.data() << std::endl;
  return o;
}

// bool operator< (const ExampleWithComponent& p1, const ExampleWithComponent&
// p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}
