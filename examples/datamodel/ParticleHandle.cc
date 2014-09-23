// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "Particle.h"
#include "ParticleCollection.h"

  const int& ParticleHandle::ID() const { return m_container->at(m_index).ID();}
  const LorentzVectorHandle& ParticleHandle::P4() const { return m_container->at(m_index).P4();}

  void ParticleHandle::setID(int value){ m_container->at(m_index).setID(value);}
  void ParticleHandle::setP4(LorentzVectorHandle value){ m_container->at(m_index).setP4(value);}


bool  ParticleHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
//  else if (m_registry != nullptr){
//    m_registry->getPODAddressFromID(m_containerID,m_container);
//    return true;
//  }
  return false;
}

void ParticleHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void ParticleHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


ParticleHandle::ParticleHandle(int index, int containerID, std::vector<Particle>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}
