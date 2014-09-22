// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "LorentzVector.h"
#include "LorentzVectorCollection.h"

  const float& LorentzVectorHandle::Phi() const { return m_container->at(m_index).Phi();}
  const float& LorentzVectorHandle::Eta() const { return m_container->at(m_index).Eta();}
  const float& LorentzVectorHandle::Mass() const { return m_container->at(m_index).Mass();}
  const float& LorentzVectorHandle::Pt() const { return m_container->at(m_index).Pt();}

  void LorentzVectorHandle::setPhi(float value){ m_container->at(m_index).setPhi(value);}
  void LorentzVectorHandle::setEta(float value){ m_container->at(m_index).setEta(value);}
  void LorentzVectorHandle::setMass(float value){ m_container->at(m_index).setMass(value);}
  void LorentzVectorHandle::setPt(float value){ m_container->at(m_index).setPt(value);}


bool  LorentzVectorHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
//  else if (m_registry != nullptr){
//    m_registry->getPODAddressFromID(m_containerID,m_container);
//    return true;
//  }
  return false;
}

void LorentzVectorHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void LorentzVectorHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


LorentzVectorHandle::LorentzVectorHandle(int index, int containerID, std::vector<LorentzVector>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}
