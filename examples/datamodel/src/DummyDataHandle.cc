// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "datamodel/DummyData.h"
#include "datamodel/DummyDataCollection.h"

  const int& DummyDataHandle::Number() const { return m_container->at(m_index).Number;}

  void DummyDataHandle::setNumber(int value){ m_container->at(m_index).Number = value;}



bool  DummyDataHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
//  else if (m_registry != nullptr){
//    m_registry->getPODAddressFromID(m_containerID,m_container);
//    return true;
//  }
  return false;
}

void DummyDataHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void DummyDataHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


DummyDataHandle::DummyDataHandle(int index, int containerID, std::vector<DummyData>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const DummyDataHandle& p1, const DummyDataHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
