// standard includes
#include <stdexcept>
#include "ExampleClusterCollection.h" 


#include "ExampleWithOneRelationCollection.h"

ExampleWithOneRelationCollection::ExampleWithOneRelationCollection() : m_collectionID(0), m_entries() ,m_rel_cluster(new std::vector<ConstExampleCluster>()),m_refCollections(nullptr), m_data(new ExampleWithOneRelationDataContainer() ) {
    m_refCollections = new podio::CollRefCollection();
  m_refCollections->push_back(new std::vector<podio::ObjectID>());

}

const ExampleWithOneRelation ExampleWithOneRelationCollection::operator[](unsigned int index) const {
  return ExampleWithOneRelation(m_entries[index]);
}

const ExampleWithOneRelation ExampleWithOneRelationCollection::at(unsigned int index) const {
  return ExampleWithOneRelation(m_entries.at(index));
}

int  ExampleWithOneRelationCollection::size() const {
  return m_entries.size();
}

ExampleWithOneRelation ExampleWithOneRelationCollection::create(){
  auto obj = new ExampleWithOneRelationObj();
  m_entries.emplace_back(obj);

  obj->id = {int(m_entries.size()-1),m_collectionID};
  return ExampleWithOneRelation(obj);
}

void ExampleWithOneRelationCollection::clear(){
  m_data->clear();
  for (auto& pointer : (*m_refCollections)) {pointer->clear(); }
  for (auto& item : (*m_rel_cluster)) {item.unlink(); }
  m_rel_cluster->clear();

  for (auto& obj : m_entries) { delete obj; }
  m_entries.clear();
}

void ExampleWithOneRelationCollection::prepareForWrite(){
  int index = 0;
  auto size = m_entries.size();
  m_data->reserve(size);
  for (auto& obj : m_entries) {m_data->push_back(obj->data); }
  if (m_refCollections != nullptr) {
    for (auto& pointer : (*m_refCollections)) {pointer->clear(); }
  }
  
  for(int i=0, size = m_data->size(); i != size; ++i){
  
  }
    for (auto& obj : m_entries) {
if (obj->m_cluster != nullptr){
(*m_refCollections)[0]->emplace_back(obj->m_cluster->getObjectID());} else {(*m_refCollections)[0]->push_back({-2,-2}); } };

}

void ExampleWithOneRelationCollection::prepareAfterRead(){
  int index = 0;
  for (auto& data : *m_data){
    auto obj = new ExampleWithOneRelationObj({index,m_collectionID}, data);
    
    m_entries.emplace_back(obj);
    ++index;
  }
}

bool ExampleWithOneRelationCollection::setReferences(const podio::ICollectionProvider* collectionProvider){

  for(unsigned int i=0, size=m_entries.size();i!=size;++i ) {
    auto id = (*(*m_refCollections)[0])[i];
    if (id.index != podio::ObjectID::invalid) {
      CollectionBase* coll = nullptr;
      collectionProvider->get(id.collectionID,coll);
      ExampleClusterCollection* tmp_coll = static_cast<ExampleClusterCollection*>(coll);
      m_entries[i]->m_cluster = new ConstExampleCluster((*tmp_coll)[id.index]);
    } else {
      m_entries[i]->m_cluster = nullptr;
    }
  }

  return true; //TODO: check success
}

void ExampleWithOneRelationCollection::push_back(ConstExampleWithOneRelation object){
    int size = m_entries.size();
    auto obj = object.m_obj;
    if (obj->id.index == podio::ObjectID::untracked) {
        obj->id = {size,m_collectionID};
        m_entries.push_back(obj);
        
    } else {
      throw std::invalid_argument( "Object already in a collection. Cannot add it to a second collection " );

    }
}

void ExampleWithOneRelationCollection::setBuffer(void* address){
  m_data = static_cast<ExampleWithOneRelationDataContainer*>(address);
}


const ExampleWithOneRelation ExampleWithOneRelationCollectionIterator::operator* () const {
  m_object.m_obj = (*m_collection)[m_index];
  return m_object;
}

const ExampleWithOneRelation* ExampleWithOneRelationCollectionIterator::operator-> () const {
    m_object.m_obj = (*m_collection)[m_index];
    return &m_object;
}

const ExampleWithOneRelationCollectionIterator& ExampleWithOneRelationCollectionIterator::operator++() const {
  ++m_index;
 return *this;
}
