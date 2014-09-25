#include "JetParticleAssociationCollection.h"

JetParticleAssociationCollection::JetParticleAssociationCollection() : m_collectionID(0), m_data(new JetParticleAssociationVector() ){
}

const JetParticleAssociationHandle& JetParticleAssociationCollection::get(int index) const{
  return m_handles[index];
}

JetParticleAssociationHandle& JetParticleAssociationCollection::create(){
    m_data->emplace_back(JetParticleAssociation());
    int index = m_data->size()-1;
    m_handles.emplace_back(JetParticleAssociationHandle(index,m_collectionID, m_data));
    return m_handles.back();
}

void JetParticleAssociationCollection::clear(){
  m_data->clear();
  m_handles.clear();

}

void JetParticleAssociationCollection::prepareForWrite(const albers::Registry* registry){
  for(auto& data : *m_data){
     data.Jet.prepareForWrite(registry);
    data.Particle.prepareForWrite(registry);
  }
}

void JetParticleAssociationCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  for (auto& data : *m_data){
    data.Jet.prepareAfterRead(registry);
data.Particle.prepareAfterRead(registry);

    m_handles.emplace_back(JetParticleAssociationHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void JetParticleAssociationCollection::setPODsAddress(const void* address){
  m_data = (JetParticleAssociationVector*)address;
}


const JetParticleAssociationHandle JetParticleAssociationCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}