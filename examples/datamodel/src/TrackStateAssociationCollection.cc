#include "datamodel/TrackStateAssociationCollection.h"

TrackStateAssociationCollection::TrackStateAssociationCollection() : m_collectionID(0), m_data(new TrackStateAssociationVector() ){
}

const TrackStateAssociationHandle& TrackStateAssociationCollection::get(int index) const{
  return m_handles[index];
}

TrackStateAssociationHandle& TrackStateAssociationCollection::create() {
  m_data->emplace_back(TrackStateAssociation());
  int index = m_data->size()-1;
  m_handles.emplace_back(TrackStateAssociationHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();

  return tmp_handle;
}

TrackStateAssociationHandle& TrackStateAssociationCollection::insert(const TrackStateAssociationHandle& origin) {
  m_data->emplace_back(origin.read());
  int index = m_data->size()-1;
  m_handles.emplace_back(TrackStateAssociationHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();

  return tmp_handle;
}  

void TrackStateAssociationCollection::clear(){
  m_data->clear();
  m_handles.clear();

}

void TrackStateAssociationCollection::prepareForWrite(const albers::Registry* registry){
  for(auto& data : *m_data){
     data.Track.prepareForWrite(registry);
    data.State.prepareForWrite(registry);
  }
}

void TrackStateAssociationCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  // fix. otherwise, m_collectionID == 0..
  m_collectionID = registry->getIDFromPODAddress( _getBuffer() );
  for (auto& data : *m_data){
    data.Track.prepareAfterRead(registry);
data.State.prepareAfterRead(registry);

    m_handles.emplace_back(TrackStateAssociationHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void TrackStateAssociationCollection::setPODsAddress(const void* address){
  m_data = (TrackStateAssociationVector*)address;
}


const TrackStateAssociationHandle TrackStateAssociationCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

//std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections() {
//}


void TrackStateAssociationCollection::print() const {
  std::cout<<"collection "<<m_collectionID
           <<", buf "<<m_data
           <<", nhandles "<<m_handles.size()<<std::endl;
}

