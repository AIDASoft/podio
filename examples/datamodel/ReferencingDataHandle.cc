// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "ReferencingData.h"
#include "ReferencingDataCollection.h"

  const DummyDataHandle& ReferencingDataHandle::DummyData() const { return m_container->at(m_index).DummyData();}

  void ReferencingDataHandle::setDummyData(DummyDataHandle value){ m_container->at(m_index).setDummyData(value);}


bool  ReferencingDataHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  } else if (m_registry != nullptr){
    m_registry->getPODAddressFromID(m_containerID,m_container);
    return true;
  }
  return false;
}

void ReferencingDataHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void ReferencingDataHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
}


ReferencingDataHandle::ReferencingDataHandle(int index, int containerID, std::vector<ReferencingData>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}

