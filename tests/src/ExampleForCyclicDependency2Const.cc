// datamodel specific includes
#include "ExampleForCyclicDependency2.h"
#include "ExampleForCyclicDependency2Const.h"
#include "ExampleForCyclicDependency2Obj.h"
#include "ExampleForCyclicDependency2Data.h"
#include "ExampleForCyclicDependency2Collection.h"
#include <iostream>
#include "ExampleForCyclicDependency1.h"




ConstExampleForCyclicDependency2::ConstExampleForCyclicDependency2() : m_obj(new ExampleForCyclicDependency2Obj()) {
 m_obj->acquire();
}



ConstExampleForCyclicDependency2::ConstExampleForCyclicDependency2(const ConstExampleForCyclicDependency2& other) : m_obj(other.m_obj) {
  m_obj->acquire();
}

ConstExampleForCyclicDependency2& ConstExampleForCyclicDependency2::operator=(const ConstExampleForCyclicDependency2& other) {
  if ( m_obj != nullptr) m_obj->release();
  m_obj = other.m_obj;
  return *this;
}

ConstExampleForCyclicDependency2::ConstExampleForCyclicDependency2(ExampleForCyclicDependency2Obj* obj) : m_obj(obj) {
  if(m_obj != nullptr)
    m_obj->acquire();
}

ConstExampleForCyclicDependency2 ConstExampleForCyclicDependency2::clone() const {
  return {new ExampleForCyclicDependency2Obj(*m_obj)};
}

ConstExampleForCyclicDependency2::~ConstExampleForCyclicDependency2(){
  if ( m_obj != nullptr) m_obj->release();
}

  /// Access the  a ref
  const ::ConstExampleForCyclicDependency1 ConstExampleForCyclicDependency2::ref() const {
    if (m_obj->m_ref == nullptr) {
      return ::ConstExampleForCyclicDependency1(nullptr);
    }
    return ::ConstExampleForCyclicDependency1(*(m_obj->m_ref));}


bool  ConstExampleForCyclicDependency2::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const podio::ObjectID ConstExampleForCyclicDependency2::getObjectID() const {
  if (m_obj !=nullptr){
    return m_obj->id;
  }
  return podio::ObjectID{-2,-2};
}

bool ConstExampleForCyclicDependency2::operator==(const ExampleForCyclicDependency2& other) const {
     return (m_obj==other.m_obj);
}

//bool operator< (const ExampleForCyclicDependency2& p1, const ExampleForCyclicDependency2& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}


