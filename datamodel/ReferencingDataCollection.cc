#include "ReferencingDataCollection.h"

ReferencingDataCollection::ReferencingDataCollection() : m_data(new ReferencingDataVector() ){
}

ReferencingDataHandle ReferencingDataCollection::get(int index) const{
  return m_handles[index];
}

ReferencingDataHandle ReferencingDataCollection::create(){
    m_data->emplace_back(ReferencingData());
    int index = m_data->size()-1;
    m_handles.emplace_back(ReferencingDataHandle(index,m_collectionID, m_data));
    return m_handles.back();
}

void ReferencingDataCollection::clear(){
  m_data->clear();
  m_handles.clear(); 

}

void ReferencingDataCollection::prepareForWrite(const albers::Registry* registry){
  for(auto& data : *m_data){
     data.m_DummyData.prepareForWrite(registry);
  }
}

void ReferencingDataCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  for (auto& data : *m_data){
    data.m_DummyData.prepareAfterRead(registry);

    m_handles.emplace_back(ReferencingDataHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void ReferencingDataCollection::setPODsAddress(const void* address){
  m_data = (ReferencingDataVector*)address;
}


const ReferencingDataHandle ReferencingDataCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}