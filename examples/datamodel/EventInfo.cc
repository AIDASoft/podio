// datamodel specific includes
#include "EventInfoObj.h"
#include "EventInfoData.h"
#include "EventInfoCollection.h"
#include <iostream>

EventInfo::EventInfo() : m_obj(new EventInfoObj()){
 m_obj->increaseRefCount();

};

EventInfo::EventInfo(const EventInfo& other) : m_obj(other.m_obj) {
  m_obj->increaseRefCount();
}

EventInfo& EventInfo::operator=(const EventInfo& other){
  if ( m_obj != nullptr) m_obj->decreaseRefCount();
  m_obj = other.m_obj;
  return *this;
}

EventInfo::EventInfo(EventInfoObj* obj) : m_obj(obj){
  if(m_obj != nullptr)
    m_obj->increaseRefCount();
}

EventInfo EventInfo::clone() const {
  return {new EventInfoObj(*m_obj)};
}

EventInfo::~EventInfo(){
  if ( m_obj != nullptr) m_obj->decreaseRefCount();
}



bool  EventInfo::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const albers::ObjectID EventInfo::getObjectID() const {
  return m_obj->id;
}


//bool operator< (const EventInfo& p1, const EventInfo& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}
