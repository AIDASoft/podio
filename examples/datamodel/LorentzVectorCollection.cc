#include "LorentzVectorCollection.h"

LorentzVectorCollection::LorentzVectorCollection() : m_collectionID(0), m_data(new LorentzVectorVector() ){
}

const LorentzVectorHandle& LorentzVectorCollection::get(int index) const{
  return m_handles[index];
}

LorentzVectorHandle& LorentzVectorCollection::create(){
    m_data->emplace_back(LorentzVector());
    int index = m_data->size()-1;
    m_handles.emplace_back(LorentzVectorHandle(index,m_collectionID, m_data));
    return m_handles.back();
}

void LorentzVectorCollection::clear(){
  m_data->clear();
  m_handles.clear();

}

void LorentzVectorCollection::prepareForWrite(const albers::Registry* registry){

}

void LorentzVectorCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  for (auto& data : *m_data){
    
    m_handles.emplace_back(LorentzVectorHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void LorentzVectorCollection::setPODsAddress(const void* address){
  m_data = (LorentzVectorVector*)address;
}


const LorentzVectorHandle LorentzVectorCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}