// datamodel specific includes
#include "ExampleForCyclicDependency1.h"
#include "ExampleForCyclicDependency1Obj.h"
#include "ExampleForCyclicDependency1Data.h"
#include "ExampleForCyclicDependency1Collection.h"
#include <iostream>
#include "ExampleForCyclicDependency2.h"




ExampleForCyclicDependency1::ExampleForCyclicDependency1() : m_obj(new ExampleForCyclicDependency1Obj()){
 m_obj->acquire();
}



ExampleForCyclicDependency1::ExampleForCyclicDependency1(const ExampleForCyclicDependency1& other) : m_obj(other.m_obj) {
  m_obj->acquire();
}

ExampleForCyclicDependency1& ExampleForCyclicDependency1::operator=(const ExampleForCyclicDependency1& other) {
  if ( m_obj != nullptr) m_obj->release();
  m_obj = other.m_obj;
  return *this;
}

ExampleForCyclicDependency1::ExampleForCyclicDependency1(ExampleForCyclicDependency1Obj* obj) : m_obj(obj){
  if(m_obj != nullptr)
    m_obj->acquire();
}

ExampleForCyclicDependency1 ExampleForCyclicDependency1::clone() const {
  return {new ExampleForCyclicDependency1Obj(*m_obj)};
}

ExampleForCyclicDependency1::~ExampleForCyclicDependency1(){
  if ( m_obj != nullptr) m_obj->release();
}

  const ::ExampleForCyclicDependency2 ExampleForCyclicDependency1::ref() const {
    if (m_obj->m_ref == nullptr) {
      return ::ExampleForCyclicDependency2(nullptr);
    }
    return ::ExampleForCyclicDependency2(*(m_obj->m_ref));
  }
void ExampleForCyclicDependency1::ref(::ExampleForCyclicDependency2 value) {
  if (m_obj->m_ref != nullptr) delete m_obj->m_ref;
  m_obj->m_ref = new ExampleForCyclicDependency2(value);
}



bool  ExampleForCyclicDependency1::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const podio::ObjectID ExampleForCyclicDependency1::getObjectID() const {
  if (m_obj !=nullptr){
    return m_obj->id;
  }
  return podio::ObjectID{-2,-2};
}


//bool operator< (const ExampleForCyclicDependency1& p1, const ExampleForCyclicDependency1& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}


