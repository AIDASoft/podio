// datamodel specific includes
#include "ExampleWithVectorMember.h"
#include "ExampleWithVectorMemberConst.h"
#include "ExampleWithVectorMemberObj.h"
#include "ExampleWithVectorMemberData.h"
#include "ExampleWithVectorMemberCollection.h"
#include <iostream>

ConstExampleWithVectorMember::ConstExampleWithVectorMember() : m_obj(new ExampleWithVectorMemberObj()){
 m_obj->acquire();
};



ConstExampleWithVectorMember::ConstExampleWithVectorMember(const ConstExampleWithVectorMember& other) : m_obj(other.m_obj) {
  m_obj->acquire();
}

ConstExampleWithVectorMember& ConstExampleWithVectorMember::operator=(const ConstExampleWithVectorMember& other) {
  if ( m_obj != nullptr) m_obj->release();
  m_obj = other.m_obj;
  return *this;
}

ConstExampleWithVectorMember::ConstExampleWithVectorMember(ExampleWithVectorMemberObj* obj) : m_obj(obj){
  if(m_obj != nullptr)
    m_obj->acquire();
}

ConstExampleWithVectorMember ConstExampleWithVectorMember::clone() const {
  return {new ExampleWithVectorMemberObj(*m_obj)};
}

ConstExampleWithVectorMember::~ConstExampleWithVectorMember(){
  if ( m_obj != nullptr) m_obj->release();
}


std::vector<int>::const_iterator ConstExampleWithVectorMember::count_begin() const {
  auto ret_value = m_obj->m_count->begin();
  std::advance(ret_value, m_obj->data.count_begin);
  return ret_value;
}

std::vector<int>::const_iterator ConstExampleWithVectorMember::count_end() const {
  auto ret_value = m_obj->m_count->begin();
  std::advance(ret_value, m_obj->data.count_end-1);
  return ++ret_value;
}

bool  ConstExampleWithVectorMember::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const podio::ObjectID ConstExampleWithVectorMember::getObjectID() const {
  return m_obj->id;
}

bool ConstExampleWithVectorMember::operator==(const ExampleWithVectorMember& other) const {
     return (m_obj==other.m_obj);
}

//bool operator< (const ExampleWithVectorMember& p1, const ExampleWithVectorMember& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}
