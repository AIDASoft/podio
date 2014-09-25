// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "JetParticleAssociation.h"
#include "JetParticleAssociationCollection.h"

  const JetHandle& JetParticleAssociationHandle::Jet() const { return m_container->at(m_index).Jet;}
  const ParticleHandle& JetParticleAssociationHandle::Particle() const { return m_container->at(m_index).Particle;}

  void JetParticleAssociationHandle::setJet(JetHandle value){ m_container->at(m_index).Jet = value;}
  void JetParticleAssociationHandle::setParticle(ParticleHandle value){ m_container->at(m_index).Particle = value;}


bool  JetParticleAssociationHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
//  else if (m_registry != nullptr){
//    m_registry->getPODAddressFromID(m_containerID,m_container);
//    return true;
//  }
  return false;
}

void JetParticleAssociationHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void JetParticleAssociationHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


JetParticleAssociationHandle::JetParticleAssociationHandle(int index, int containerID, std::vector<JetParticleAssociation>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}
