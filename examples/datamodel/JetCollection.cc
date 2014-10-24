#include "JetCollection.h"

JetCollection::JetCollection() : m_collectionID(0), m_data(new JetVector() ){

  m_rel_particles = new std::vector<ParticleHandle>();
}

const JetHandle& JetCollection::get(int index) const{
  return m_handles[index];
}

JetHandle& JetCollection::create(){
  m_data->emplace_back(Jet());
  int index = m_data->size()-1;
  m_handles.emplace_back(JetHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();
  auto particles_tmp = new std::vector<ParticleHandle>();
  m_rel_particles_tmp.push_back(particles_tmp);
  tmp_handle.m_particles = particles_tmp;

  return tmp_handle;
}

void JetCollection::clear(){
  m_data->clear();
  m_handles.clear();
for (auto& pointer : m_rel_particles_tmp) {delete pointer;}
m_rel_particles_tmp.clear();

}

void JetCollection::prepareForWrite(const albers::Registry* registry){
  int counter(0);
  for (int i=0, size = m_data->size(); i<size; ++i) {
    auto& tmp = m_rel_particles_tmp[i];
    int particles_size = tmp->size();
    m_rel_particles->insert(m_rel_particles->end(),
                            std::make_move_iterator( tmp->begin() ),
                            std::make_move_iterator( tmp->end() )
			    );   
    (*m_data)[i].particles_begin = counter;
    counter += particles_size;
    (*m_data)[i].particles_end = counter;
  }
}

void JetCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  for (auto& data : *m_data){
    
    m_handles.emplace_back(JetHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void JetCollection::setPODsAddress(const void* address){
  m_data = (JetVector*)address;
}


const JetHandle JetCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

//std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections() {
//}


