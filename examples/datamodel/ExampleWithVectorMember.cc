// datamodel specific includes
#include "ExampleWithVectorMemberObj.h"
#include "ExampleWithVectorMemberData.h"
#include "ExampleWithVectorMemberCollection.h"
#include <iostream>

ExampleWithVectorMember::ExampleWithVectorMember() : m_obj(new ExampleWithVectorMemberObj()){
 m_obj->acquire();
};

ExampleWithVectorMember::ExampleWithVectorMember(std::vector<int> count) : m_obj(new ExampleWithVectorMemberObj()){
 m_obj->acquire();
   m_obj->data.count = count;
};

ExampleWithVectorMember::ExampleWithVectorMember(const ExampleWithVectorMember& other) : m_obj(other.m_obj) {
  m_obj->acquire();
}

ExampleWithVectorMember& ExampleWithVectorMember::operator=(const ExampleWithVectorMember& other){
  if ( m_obj != nullptr) m_obj->release();
  m_obj = other.m_obj;
  return *this;
}

ExampleWithVectorMember::ExampleWithVectorMember(ExampleWithVectorMemberObj* obj) : m_obj(obj){
  if(m_obj != nullptr)
    m_obj->acquire();
}

ExampleWithVectorMember ExampleWithVectorMember::clone() const {
  return {new ExampleWithVectorMemberObj(*m_obj)};
}

ExampleWithVectorMember::~ExampleWithVectorMember(){
  if ( m_obj != nullptr) m_obj->release();
}

void ExampleWithVectorMember::count(class std::vector<int> value){ m_obj->data.count = value;}


bool  ExampleWithVectorMember::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const albers::ObjectID ExampleWithVectorMember::getObjectID() const {
  return m_obj->id;
}


//bool operator< (const ExampleWithVectorMember& p1, const ExampleWithVectorMember& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}
