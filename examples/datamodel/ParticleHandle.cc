// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "Particle.h"
#include "ParticleCollection.h"

  const float& ParticleHandle::Pz() const { return m_container->at(m_index).Pz();}
  const float& ParticleHandle::Px() const { return m_container->at(m_index).Px();}
  const float& ParticleHandle::Py() const { return m_container->at(m_index).Py();}
  const float& ParticleHandle::Energy() const { return m_container->at(m_index).Energy();}
  const int& ParticleHandle::Charge() const { return m_container->at(m_index).Charge();}

  void ParticleHandle::setPz(float value){ m_container->at(m_index).setPz(value);}
  void ParticleHandle::setPx(float value){ m_container->at(m_index).setPx(value);}
  void ParticleHandle::setPy(float value){ m_container->at(m_index).setPy(value);}
  void ParticleHandle::setEnergy(float value){ m_container->at(m_index).setEnergy(value);}
  void ParticleHandle::setCharge(int value){ m_container->at(m_index).setCharge(value);}


bool  ParticleHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  } else if (m_registry != nullptr){
    m_registry->getPODAddressFromID(m_containerID,m_container);
    return true;
  }
  return false;
}

void ParticleHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void ParticleHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
}


ParticleHandle::ParticleHandle(int index, int containerID, std::vector<Particle>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}

