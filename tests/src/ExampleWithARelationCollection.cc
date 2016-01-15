// standard includes
#include <stdexcept>
#include "ExampleWithNamespaceCollection.h" 


#include "ExampleWithARelationCollection.h"

namespace ex {

ExampleWithARelationCollection::ExampleWithARelationCollection() : m_collectionID(0), m_entries() ,m_rel_ref(new std::vector<ex::ConstExampleWithNamespace>()),m_refCollections(nullptr), m_data(new ExampleWithARelationDataContainer() ) {
    m_refCollections = new podio::CollRefCollection();
  m_refCollections->push_back(new std::vector<podio::ObjectID>());

}

const ExampleWithARelation ExampleWithARelationCollection::operator[](unsigned int index) const {
  return ExampleWithARelation(m_entries[index]);
}

const ExampleWithARelation ExampleWithARelationCollection::at(unsigned int index) const {
  return ExampleWithARelation(m_entries.at(index));
}

int  ExampleWithARelationCollection::size() const {
  return m_entries.size();
}

ExampleWithARelation ExampleWithARelationCollection::create(){
  auto obj = new ExampleWithARelationObj();
  m_entries.emplace_back(obj);

  obj->id = {int(m_entries.size()-1),m_collectionID};
  return ExampleWithARelation(obj);
}

void ExampleWithARelationCollection::clear(){
  m_data->clear();
  for (auto& pointer : (*m_refCollections)) {pointer->clear(); }
  for (auto& item : (*m_rel_ref)) {item.unlink(); }
  m_rel_ref->clear();

  for (auto& obj : m_entries) { delete obj; }
  m_entries.clear();
}

void ExampleWithARelationCollection::prepareForWrite(){
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
if (obj->m_ref != nullptr){
(*m_refCollections)[0]->emplace_back(obj->m_ref->getObjectID());} else {(*m_refCollections)[0]->push_back({-2,-2}); } }

}

void ExampleWithARelationCollection::prepareAfterRead(){
  int index = 0;
  for (auto& data : *m_data){
    auto obj = new ExampleWithARelationObj({index,m_collectionID}, data);
    
    m_entries.emplace_back(obj);
    ++index;
  }
}

bool ExampleWithARelationCollection::setReferences(const podio::ICollectionProvider* collectionProvider){

  for(unsigned int i=0, size=m_entries.size();i!=size;++i ) {
    auto id = (*(*m_refCollections)[0])[i];
    if (id.index != podio::ObjectID::invalid) {
      CollectionBase* coll = nullptr;
      collectionProvider->get(id.collectionID,coll);
      ex::ExampleWithNamespaceCollection* tmp_coll = static_cast<ex::ExampleWithNamespaceCollection*>(coll);
      m_entries[i]->m_ref = new ConstExampleWithNamespace((*tmp_coll)[id.index]);
    } else {
      m_entries[i]->m_ref = nullptr;
    }
  }

  return true; //TODO: check success
}

void ExampleWithARelationCollection::push_back(ConstExampleWithARelation object){
    int size = m_entries.size();
    auto obj = object.m_obj;
    if (obj->id.index == podio::ObjectID::untracked) {
        obj->id = {size,m_collectionID};
        m_entries.push_back(obj);
        
    } else {
      throw std::invalid_argument( "Object already in a collection. Cannot add it to a second collection " );

    }
}

void ExampleWithARelationCollection::setBuffer(void* address){
  m_data = static_cast<ExampleWithARelationDataContainer*>(address);
}


const ExampleWithARelation ExampleWithARelationCollectionIterator::operator* () const {
  m_object.m_obj = (*m_collection)[m_index];
  return m_object;
}

const ExampleWithARelation* ExampleWithARelationCollectionIterator::operator-> () const {
    m_object.m_obj = (*m_collection)[m_index];
    return &m_object;
}

const ExampleWithARelationCollectionIterator& ExampleWithARelationCollectionIterator::operator++() const {
  ++m_index;
 return *this;
}

} // namespace ex
