// datamodel specific includes
#include "EventInfo.h"
#include "EventInfoConst.h"
#include "EventInfoObj.h"
#include "EventInfoData.h"
#include "EventInfoCollection.h"
#include <iostream>




ConstEventInfo::ConstEventInfo() : m_obj(new EventInfoObj()) {
 m_obj->acquire();
}

ConstEventInfo::ConstEventInfo(int Number) : m_obj(new EventInfoObj()){
 m_obj->acquire();
   m_obj->data.Number = Number;
}


ConstEventInfo::ConstEventInfo(const ConstEventInfo& other) : m_obj(other.m_obj) {
  m_obj->acquire();
}

ConstEventInfo& ConstEventInfo::operator=(const ConstEventInfo& other) {
  if ( m_obj != nullptr) m_obj->release();
  m_obj = other.m_obj;
  return *this;
}

ConstEventInfo::ConstEventInfo(EventInfoObj* obj) : m_obj(obj) {
  if(m_obj != nullptr)
    m_obj->acquire();
}

ConstEventInfo ConstEventInfo::clone() const {
  return {new EventInfoObj(*m_obj)};
}

ConstEventInfo::~ConstEventInfo(){
  if ( m_obj != nullptr) m_obj->release();
}

  /// Access the  event number
  const int& ConstEventInfo::Number() const { return m_obj->data.Number; }


int ConstEventInfo::getNumber() const { return Number(); } 
bool  ConstEventInfo::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const podio::ObjectID ConstEventInfo::getObjectID() const {
  if (m_obj !=nullptr){
    return m_obj->id;
  }
  return podio::ObjectID{-2,-2};
}

bool ConstEventInfo::operator==(const EventInfo& other) const {
     return (m_obj==other.m_obj);
}

//bool operator< (const EventInfo& p1, const EventInfo& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}


