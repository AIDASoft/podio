// datamodel specific includes
#include "EventInfo.h"
#include "EventInfoCollection.h"
#include "EventInfoConst.h"
#include "EventInfoData.h"
#include "EventInfoObj.h"
#include <iostream>

EventInfo::EventInfo() : m_obj(new EventInfoObj()) { m_obj->acquire(); }

EventInfo::EventInfo(int Number) : m_obj(new EventInfoObj()) {
  m_obj->acquire();
  m_obj->data.Number = Number;
}

EventInfo::EventInfo(const EventInfo &other) : m_obj(other.m_obj) {
  m_obj->acquire();
}

EventInfo &EventInfo::operator=(const EventInfo &other) {
  if (m_obj != nullptr)
    m_obj->release();
  m_obj = other.m_obj;
  return *this;
}

EventInfo::EventInfo(EventInfoObj *obj) : m_obj(obj) {
  if (m_obj != nullptr)
    m_obj->acquire();
}

EventInfo EventInfo::clone() const { return {new EventInfoObj(*m_obj)}; }

EventInfo::~EventInfo() {
  if (m_obj != nullptr)
    m_obj->release();
}

EventInfo::operator ConstEventInfo() const { return ConstEventInfo(m_obj); }

const int &EventInfo::Number() const { return m_obj->data.Number; }

void EventInfo::Number(int value) { m_obj->data.Number = value; }

int EventInfo::getNumber() const { return Number(); }
bool EventInfo::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const podio::ObjectID EventInfo::getObjectID() const {
  if (m_obj != nullptr) {
    return m_obj->id;
  }
  return podio::ObjectID{-2, -2};
}

bool EventInfo::operator==(const ConstEventInfo &other) const {
  return (m_obj == other.m_obj);
}

std::ostream &operator<<(std::ostream &o, const ConstEventInfo &value) {
  o << " id : " << value.id() << std::endl;
  o << " Number : " << value.Number() << std::endl;
  return o;
}

// bool operator< (const EventInfo& p1, const EventInfo& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}
