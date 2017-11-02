// datamodel specific includes
#include "ExampleForCyclicDependency2.h"
#include "ExampleForCyclicDependency1.h"
#include "ExampleForCyclicDependency2Collection.h"
#include "ExampleForCyclicDependency2Const.h"
#include "ExampleForCyclicDependency2Data.h"
#include "ExampleForCyclicDependency2Obj.h"
#include <iostream>

ExampleForCyclicDependency2::ExampleForCyclicDependency2()
    : m_obj(new ExampleForCyclicDependency2Obj()) {
  m_obj->acquire();
}

ExampleForCyclicDependency2::ExampleForCyclicDependency2(
    const ExampleForCyclicDependency2 &other)
    : m_obj(other.m_obj) {
  m_obj->acquire();
}

ExampleForCyclicDependency2 &ExampleForCyclicDependency2::
operator=(const ExampleForCyclicDependency2 &other) {
  if (m_obj != nullptr)
    m_obj->release();
  m_obj = other.m_obj;
  return *this;
}

ExampleForCyclicDependency2::ExampleForCyclicDependency2(
    ExampleForCyclicDependency2Obj *obj)
    : m_obj(obj) {
  if (m_obj != nullptr)
    m_obj->acquire();
}

ExampleForCyclicDependency2 ExampleForCyclicDependency2::clone() const {
  return {new ExampleForCyclicDependency2Obj(*m_obj)};
}

ExampleForCyclicDependency2::~ExampleForCyclicDependency2() {
  if (m_obj != nullptr)
    m_obj->release();
}

ExampleForCyclicDependency2::operator ConstExampleForCyclicDependency2() const {
  return ConstExampleForCyclicDependency2(m_obj);
}

const ::ConstExampleForCyclicDependency1
ExampleForCyclicDependency2::ref() const {
  if (m_obj->m_ref == nullptr) {
    return ::ConstExampleForCyclicDependency1(nullptr);
  }
  return ::ConstExampleForCyclicDependency1(*(m_obj->m_ref));
}
void ExampleForCyclicDependency2::ref(
    ::ConstExampleForCyclicDependency1 value) {
  if (m_obj->m_ref != nullptr)
    delete m_obj->m_ref;
  m_obj->m_ref = new ConstExampleForCyclicDependency1(value);
}

bool ExampleForCyclicDependency2::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const podio::ObjectID ExampleForCyclicDependency2::getObjectID() const {
  if (m_obj != nullptr) {
    return m_obj->id;
  }
  return podio::ObjectID{-2, -2};
}

bool ExampleForCyclicDependency2::
operator==(const ConstExampleForCyclicDependency2 &other) const {
  return (m_obj == other.m_obj);
}

std::ostream &operator<<(std::ostream &o,
                         const ConstExampleForCyclicDependency2 &value) {
  o << " id : " << value.id() << std::endl;
  o << " ref : " << value.ref().id() << std::endl;
  return o;
}

// bool operator< (const ExampleForCyclicDependency2& p1, const
// ExampleForCyclicDependency2& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}
