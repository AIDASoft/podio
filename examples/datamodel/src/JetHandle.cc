// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "datamodel/Jet.h"
#include "datamodel/JetCollection.h"



bool  JetHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
  return false;
}

void JetHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void JetHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


JetHandle::JetHandle(int index, int containerID, std::vector<Jet>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const JetHandle& p1, const JetHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
