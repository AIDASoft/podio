// datamodel specific includes
#include "ExampleWithString.h"
#include "ExampleWithStringConst.h"
#include "ExampleWithStringObj.h"
#include "ExampleWithStringData.h"
#include "ExampleWithStringCollection.h"
#include <iostream>




ConstExampleWithString::ConstExampleWithString() : m_obj(new ExampleWithStringObj()) {
 m_obj->acquire();
}

ConstExampleWithString::ConstExampleWithString(std::string theString) : m_obj(new ExampleWithStringObj()){
 m_obj->acquire();
   m_obj->data.theString = theString;
}


ConstExampleWithString::ConstExampleWithString(const ConstExampleWithString& other) : m_obj(other.m_obj) {
  m_obj->acquire();
}

ConstExampleWithString& ConstExampleWithString::operator=(const ConstExampleWithString& other) {
  if ( m_obj != nullptr) m_obj->release();
  m_obj = other.m_obj;
  return *this;
}

ConstExampleWithString::ConstExampleWithString(ExampleWithStringObj* obj) : m_obj(obj) {
  if(m_obj != nullptr)
    m_obj->acquire();
}

ConstExampleWithString ConstExampleWithString::clone() const {
  return {new ExampleWithStringObj(*m_obj)};
}

ConstExampleWithString::~ConstExampleWithString(){
  if ( m_obj != nullptr) m_obj->release();
}

  /// Access the  the string
  const std::string& ConstExampleWithString::theString() const { return m_obj->data.theString; }



bool  ConstExampleWithString::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const podio::ObjectID ConstExampleWithString::getObjectID() const {
  if (m_obj !=nullptr){
    return m_obj->id;
  }
  return podio::ObjectID{-2,-2};
}

bool ConstExampleWithString::operator==(const ExampleWithString& other) const {
     return (m_obj==other.m_obj);
}

//bool operator< (const ExampleWithString& p1, const ExampleWithString& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}


