#include "ParticleAssociationCollection.h"

ParticleAssociationCollection::ParticleAssociationCollection() : m_collectionID(0), m_data(new ParticleAssociationVector() ){
}

const ParticleAssociationHandle& ParticleAssociationCollection::get(int index) const{
  return m_handles[index];
}

ParticleAssociationHandle& ParticleAssociationCollection::create(){
    m_data->emplace_back(ParticleAssociation());
    int index = m_data->size()-1;
    m_handles.emplace_back(ParticleAssociationHandle(index,m_collectionID, m_data));
    return m_handles.back();
}

void ParticleAssociationCollection::clear(){
  m_data->clear();
  m_handles.clear();

}

void ParticleAssociationCollection::prepareForWrite(const albers::Registry* registry){
  for(auto& data : *m_data){
     data.First.prepareForWrite(registry);
    data.Second.prepareForWrite(registry);
  }
}

void ParticleAssociationCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  // fix. otherwise, m_collectionID == 0..
  m_collectionID = registry->getIDFromPODAddress( _getBuffer() );
  for (auto& data : *m_data){
    data.First.prepareAfterRead(registry);
data.Second.prepareAfterRead(registry);

    m_handles.emplace_back(ParticleAssociationHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void ParticleAssociationCollection::setPODsAddress(const void* address){
  m_data = (ParticleAssociationVector*)address;
}


const ParticleAssociationHandle ParticleAssociationCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

void ParticleAssociationCollection::print() const {
  std::cout<<"collection "<<m_collectionID
           <<", buf "<<m_data
           <<", nhandles "<<m_handles.size()<<std::endl;
}