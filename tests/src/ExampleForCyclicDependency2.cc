// datamodel specific includes
#include "ExampleForCyclicDependency2.h"
#include "ExampleForCyclicDependency2Obj.h"
#include "ExampleForCyclicDependency2Data.h"
#include "ExampleForCyclicDependency2Collection.h"
#include <iostream>
#include "ExampleForCyclicDependency1.h"




ExampleForCyclicDependency2::ExampleForCyclicDependency2() : m_obj(new ExampleForCyclicDependency2Obj()){
 m_obj->acquire();
}



ExampleForCyclicDependency2::ExampleForCyclicDependency2(const ExampleForCyclicDependency2& other) : m_obj(other.m_obj) {
  m_obj->acquire();
}

ExampleForCyclicDependency2& ExampleForCyclicDependency2::operator=(const ExampleForCyclicDependency2& other) {
  if ( m_obj != nullptr) m_obj->release();
  m_obj = other.m_obj;
  return *this;
}

ExampleForCyclicDependency2::ExampleForCyclicDependency2(ExampleForCyclicDependency2Obj* obj) : m_obj(obj){
  if(m_obj != nullptr)
    m_obj->acquire();
}

ExampleForCyclicDependency2 ExampleForCyclicDependency2::clone() const {
  return {new ExampleForCyclicDependency2Obj(*m_obj)};
}

ExampleForCyclicDependency2::~ExampleForCyclicDependency2(){
  if ( m_obj != nullptr) m_obj->release();
}

//ExampleForCyclicDependency2::operator ExampleForCyclicDependency2() const {return ExampleForCyclicDependency2(m_obj);}

  const ::ExampleForCyclicDependency1 ExampleForCyclicDependency2::ref() const {
    if (m_obj->m_ref == nullptr) {
      return ::ExampleForCyclicDependency1(nullptr);
    }
    return ::ExampleForCyclicDependency1(*(m_obj->m_ref));
  }
void ExampleForCyclicDependency2::ref(::ExampleForCyclicDependency1 value) {
  if (m_obj->m_ref != nullptr) delete m_obj->m_ref;
  m_obj->m_ref = new ExampleForCyclicDependency1(value);
}



bool  ExampleForCyclicDependency2::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const podio::ObjectID ExampleForCyclicDependency2::getObjectID() const {
  if (m_obj !=nullptr){
    return m_obj->id;
  }
  return podio::ObjectID{-2,-2};
}


//bool operator< (const ExampleForCyclicDependency2& p1, const ExampleForCyclicDependency2& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}


