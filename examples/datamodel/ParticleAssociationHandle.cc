// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "ParticleAssociation.h"
#include "ParticleAssociationCollection.h"

  const ParticleHandle& ParticleAssociationHandle::First() const { return m_container->at(m_index).First;}
  const ParticleHandle& ParticleAssociationHandle::Second() const { return m_container->at(m_index).Second;}

  void ParticleAssociationHandle::setFirst(ParticleHandle value){ m_container->at(m_index).First = value;}
  void ParticleAssociationHandle::setSecond(ParticleHandle value){ m_container->at(m_index).Second = value;}


bool  ParticleAssociationHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
//  else if (m_registry != nullptr){
//    m_registry->getPODAddressFromID(m_containerID,m_container);
//    return true;
//  }
  return false;
}

void ParticleAssociationHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void ParticleAssociationHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


ParticleAssociationHandle::ParticleAssociationHandle(int index, int containerID, std::vector<ParticleAssociation>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const ParticleAssociationHandle& p1, const ParticleAssociationHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
