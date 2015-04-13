// standard includes
#include <stdexcept>
#include "ExampleHitCollection.h" 


#include "ExampleClusterCollection.h"

ExampleClusterCollection::ExampleClusterCollection() : m_collectionID(0), m_entries() ,m_rel_Hits(new std::vector<ExampleHit>()),m_refCollections(nullptr), m_data(new ExampleClusterDataContainer() ) {
    m_refCollections = new albers::CollRefCollection();
  m_refCollections->push_back(new std::vector<albers::ObjectID>());

}

const ExampleCluster ExampleClusterCollection::operator[](int index) const {
  return ExampleCluster(m_entries[index]);
}

int  ExampleClusterCollection::size() const {
  return m_entries.size();
}

ExampleCluster ExampleClusterCollection::create(){
  auto entry = new ExampleClusterEntry();
  m_entries.emplace_back(entry);
  auto Hits_tmp = new std::vector<ExampleHit>();
  m_rel_Hits_tmp.push_back(Hits_tmp);
  entry->m_Hits = Hits_tmp;

  entry->id = {int(m_entries.size()-1),m_collectionID};
  return ExampleCluster(entry);
}

void ExampleClusterCollection::clear(){
  m_data->clear();
  for (auto& entry : m_entries) { delete entry; }
  m_entries.clear();
  for (auto& pointer : (*m_refCollections)) {pointer->clear(); }
  // clear relations to Hits. Make sure to unlink() the reference data as they may be gone already
  for (auto& pointer : m_rel_Hits_tmp) {for(auto& item : (*pointer)) {item.unlink();}; delete pointer;}
  m_rel_Hits_tmp.clear();
  for (auto& item : (*m_rel_Hits)) {item.unlink(); }
  m_rel_Hits->clear();

}

void ExampleClusterCollection::prepareForWrite(){
  int index = 0;
  auto size = m_entries.size();
  m_data->reserve(size);
  for (auto& entry : m_entries) {m_data->push_back(entry->data); }
  if (m_refCollections != nullptr) {
    for (auto& pointer : (*m_refCollections)) {pointer->clear(); }
  }
  
  for(int i=0, size = m_data->size(); i != size; ++i){
  
  (*m_data)[i].Hits_begin=index;
  (*m_data)[i].Hits_end+=index;
  index = (*m_data)[index].Hits_end;
  for(auto it : (*m_rel_Hits_tmp[i])) {
    //TODO: check validity of objects
    (*m_refCollections)[0]->emplace_back(it.getObjectID());
    m_rel_Hits->push_back(it);
  }
  }

}

void ExampleClusterCollection::prepareAfterRead(){
  int index = 0;
  for (auto& data : *m_data){
    auto entry = new ExampleClusterEntry({index,m_collectionID}, data);
        entry->m_Hits = m_rel_Hits;
    m_entries.emplace_back(entry);
    ++index;
  }
}

bool ExampleClusterCollection::setReferences(albers::Registry* registry){
  
  for(unsigned int i=0, size=(*m_refCollections)[0]->size();i!=size;++i ) {
    auto id = (*(*m_refCollections)[0])[i];
    ExampleHitCollection* tmp_coll = nullptr;
    registry->getCollectionFromID(id.collectionID,tmp_coll);
    auto tmp = (*tmp_coll)[id.index];
    m_rel_Hits->emplace_back(tmp);
  }
  return true; //TODO: check success
}

void ExampleClusterCollection::push_back(ExampleCluster object){
    int size = m_entries.size();
    auto entry = object.m_entry;
    if (entry->id.index == albers::ObjectID::untracked) {
        entry->id = {size,m_collectionID};
        m_entries.push_back(entry);
    } else {
      throw std::invalid_argument( "Cannot add an object to collection that is already owned by another collection." );

    }
}

void ExampleClusterCollection::setBuffer(void* address){
  m_data = static_cast<ExampleClusterDataContainer*>(address);
}


const ExampleCluster ExampleClusterCollectionIterator::operator* () const {
  m_object.m_entry = (*m_collection)[m_index];
  return m_object;
}

const ExampleCluster* ExampleClusterCollectionIterator::operator-> () const {
    m_object.m_entry = (*m_collection)[m_index];
    return &m_object;
}

const ExampleClusterCollectionIterator& ExampleClusterCollectionIterator::operator++() const {
  ++m_index;
 return *this;
}
