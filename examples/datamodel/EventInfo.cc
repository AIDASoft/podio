// albers specific includes
#include "albers/Registry.h"

// datamodel specific includes
#include "EventInfoEntry.h"
#include "EventInfoData.h"
#include "EventInfoCollection.h"

const int& EventInfo::Number() const { return m_entry->data.Number;}

void EventInfo::Number(int value){ m_entry->data.Number = value;}


bool  EventInfo::isAvailable() const {
  if (m_entry != nullptr) {
    return true;
  }
  return false;
}

const albers::ObjectID EventInfo::getObjectID() const {
  return m_entry->id;
}


EventInfo::EventInfo(EventInfoEntry* entry) : m_entry(entry){
  if(m_entry != nullptr)
    m_entry->increaseRefCount();
}

EventInfo& EventInfo::operator=(const EventInfo& other){
  if ( m_entry != nullptr && m_entry->decreaseRefCount()==0) {
    std::cout << "deleting free-floating EventInfo at " << m_entry << std::endl;
    delete m_entry;
  }
  m_entry = other.m_entry;
  return *this;
}

EventInfo::~EventInfo(){
  if ( m_entry != nullptr && m_entry->decreaseRefCount()==0 ){
    std::cout << "deleting free-floating EventInfo at " << m_entry << std::endl;
    delete m_entry;
   }
}

//bool operator< (const EventInfo& p1, const EventInfo& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}
