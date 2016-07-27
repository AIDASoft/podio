// datamodel specific includes
#include "ExampleForCyclicDependency1.h"
#include "ExampleForCyclicDependency1Const.h"
#include "ExampleForCyclicDependency1Obj.h"
#include "ExampleForCyclicDependency1Data.h"
#include "ExampleForCyclicDependency1Collection.h"
#include <iostream>
#include "ExampleForCyclicDependency2.h"




ConstExampleForCyclicDependency1::ConstExampleForCyclicDependency1() : m_obj(new ExampleForCyclicDependency1Obj()) {
 m_obj->acquire();
}



ConstExampleForCyclicDependency1::ConstExampleForCyclicDependency1(const ConstExampleForCyclicDependency1& other) : m_obj(other.m_obj) {
  m_obj->acquire();
}

ConstExampleForCyclicDependency1& ConstExampleForCyclicDependency1::operator=(const ConstExampleForCyclicDependency1& other) {
  if ( m_obj != nullptr) m_obj->release();
  m_obj = other.m_obj;
  return *this;
}

ConstExampleForCyclicDependency1::ConstExampleForCyclicDependency1(ExampleForCyclicDependency1Obj* obj) : m_obj(obj) {
  if(m_obj != nullptr)
    m_obj->acquire();
}

ConstExampleForCyclicDependency1 ConstExampleForCyclicDependency1::clone() const {
  return {new ExampleForCyclicDependency1Obj(*m_obj)};
}

ConstExampleForCyclicDependency1::~ConstExampleForCyclicDependency1(){
  if ( m_obj != nullptr) m_obj->release();
}

  /// Access the  a ref
  const ::ConstExampleForCyclicDependency2 ConstExampleForCyclicDependency1::ref() const {
    if (m_obj->m_ref == nullptr) {
      return ::ConstExampleForCyclicDependency2(nullptr);
    }
    return ::ConstExampleForCyclicDependency2(*(m_obj->m_ref));}


bool  ConstExampleForCyclicDependency1::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const podio::ObjectID ConstExampleForCyclicDependency1::getObjectID() const {
  if (m_obj !=nullptr){
    return m_obj->id;
  }
  return podio::ObjectID{-2,-2};
}

bool ConstExampleForCyclicDependency1::operator==(const ExampleForCyclicDependency1& other) const {
     return (m_obj==other.m_obj);
}

//bool operator< (const ExampleForCyclicDependency1& p1, const ExampleForCyclicDependency1& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}


