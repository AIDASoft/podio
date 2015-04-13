// standard includes
#include <stdexcept>
#include "ExampleClusterCollection.h" 


#include "ExampleReferencingTypeCollection.h"

ExampleReferencingTypeCollection::ExampleReferencingTypeCollection() : m_collectionID(0), m_entries() ,m_rel_Clusters(new std::vector<ExampleCluster>()),m_refCollections(nullptr), m_data(new ExampleReferencingTypeDataContainer() ) {
    m_refCollections = new albers::CollRefCollection();
  m_refCollections->push_back(new std::vector<albers::ObjectID>());

}

const ExampleReferencingType ExampleReferencingTypeCollection::operator[](int index) const {
  return ExampleReferencingType(m_entries[index]);
}

int  ExampleReferencingTypeCollection::size() const {
  return m_entries.size();
}

ExampleReferencingType ExampleReferencingTypeCollection::create(){
  auto entry = new ExampleReferencingTypeEntry();
  m_entries.emplace_back(entry);
  auto Clusters_tmp = new std::vector<ExampleCluster>();
  m_rel_Clusters_tmp.push_back(Clusters_tmp);
  entry->m_Clusters = Clusters_tmp;

  entry->id = {int(m_entries.size()-1),m_collectionID};
  return ExampleReferencingType(entry);
}

void ExampleReferencingTypeCollection::clear(){
  m_data->clear();
  for (auto& entry : m_entries) { delete entry; }
  m_entries.clear();
  for (auto& pointer : (*m_refCollections)) {pointer->clear(); }
  // clear relations to Clusters. Make sure to unlink() the reference data as they may be gone already
  for (auto& pointer : m_rel_Clusters_tmp) {for(auto& item : (*pointer)) {item.unlink();}; delete pointer;}
  m_rel_Clusters_tmp.clear();
  for (auto& item : (*m_rel_Clusters)) {item.unlink(); }
  m_rel_Clusters->clear();

}

void ExampleReferencingTypeCollection::prepareForWrite(){
  int index = 0;
  auto size = m_entries.size();
  m_data->reserve(size);
  for (auto& entry : m_entries) {m_data->push_back(entry->data); }
  if (m_refCollections != nullptr) {
    for (auto& pointer : (*m_refCollections)) {pointer->clear(); }
  }
  
  for(int i=0, size = m_data->size(); i != size; ++i){
  
  (*m_data)[i].Clusters_begin=index;
  (*m_data)[i].Clusters_end+=index;
  index = (*m_data)[index].Clusters_end;
  for(auto it : (*m_rel_Clusters_tmp[i])) {
    //TODO: check validity of objects
    (*m_refCollections)[0]->emplace_back(it.getObjectID());
    m_rel_Clusters->push_back(it);
  }
  }

}

void ExampleReferencingTypeCollection::prepareAfterRead(){
  int index = 0;
  for (auto& data : *m_data){
    auto entry = new ExampleReferencingTypeEntry({index,m_collectionID}, data);
        entry->m_Clusters = m_rel_Clusters;
    m_entries.emplace_back(entry);
    ++index;
  }
}

bool ExampleReferencingTypeCollection::setReferences(albers::Registry* registry){
  
  for(unsigned int i=0, size=(*m_refCollections)[0]->size();i!=size;++i ) {
    auto id = (*(*m_refCollections)[0])[i];
    ExampleClusterCollection* tmp_coll = nullptr;
    registry->getCollectionFromID(id.collectionID,tmp_coll);
    auto tmp = (*tmp_coll)[id.index];
    m_rel_Clusters->emplace_back(tmp);
  }
  return true; //TODO: check success
}

void ExampleReferencingTypeCollection::push_back(ExampleReferencingType object){
    int size = m_entries.size();
    auto entry = object.m_entry;
    if (entry->id.index == albers::ObjectID::untracked) {
        entry->id = {size,m_collectionID};
        m_entries.push_back(entry);
    } else {
      throw std::invalid_argument( "Cannot add an object to collection that is already owned by another collection." );

    }
}

void ExampleReferencingTypeCollection::setBuffer(void* address){
  m_data = static_cast<ExampleReferencingTypeDataContainer*>(address);
}


const ExampleReferencingType ExampleReferencingTypeCollectionIterator::operator* () const {
  m_object.m_entry = (*m_collection)[m_index];
  return m_object;
}

const ExampleReferencingType* ExampleReferencingTypeCollectionIterator::operator-> () const {
    m_object.m_entry = (*m_collection)[m_index];
    return &m_object;
}

const ExampleReferencingTypeCollectionIterator& ExampleReferencingTypeCollectionIterator::operator++() const {
  ++m_index;
 return *this;
}
