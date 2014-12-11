#include "datamodel/ParticleCollection.h"

ParticleCollection::ParticleCollection() : m_collectionID(0), m_data(new ParticleVector() ){
}

const ParticleHandle& ParticleCollection::get(int index) const{
  return m_handles[index];
}

ParticleHandle& ParticleCollection::create() {
  m_data->emplace_back(Particle());
  int index = m_data->size()-1;
  m_handles.emplace_back(ParticleHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();

  return tmp_handle;
}

ParticleHandle& ParticleCollection::insert(const ParticleHandle& origin) {
  m_data->emplace_back(origin.read());
  int index = m_data->size()-1;
  m_handles.emplace_back(ParticleHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();

  return tmp_handle;
}  

void ParticleCollection::clear(){
  m_data->clear();
  m_handles.clear();

}

void ParticleCollection::prepareForWrite(const albers::Registry* registry){

}

void ParticleCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  // fix. otherwise, m_collectionID == 0..
  m_collectionID = registry->getIDFromPODAddress( _getBuffer() );
  for (auto& data : *m_data){
    
    m_handles.emplace_back(ParticleHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void ParticleCollection::setPODsAddress(const void* address){
  m_data = (ParticleVector*)address;
}


const ParticleHandle ParticleCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

//std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections() {
//}


void ParticleCollection::print() const {
  std::cout<<"collection "<<m_collectionID
           <<", buf "<<m_data
           <<", nhandles "<<m_handles.size()<<std::endl;
}

