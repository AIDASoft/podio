// standard includes
#include <stdexcept>
#include "ExampleHitCollection.h" 


#include "ExampleClusterCollection.h"

ExampleClusterCollection::ExampleClusterCollection() : m_collectionID(0), m_entries() ,m_rel_Hits(new std::vector<ConstExampleHit>()),m_refCollections(nullptr), m_data(new ExampleClusterDataContainer() ) {
    m_refCollections = new podio::CollRefCollection();
  m_refCollections->push_back(new std::vector<podio::ObjectID>());

}

const ExampleCluster ExampleClusterCollection::operator[](int index) const {
  return ExampleCluster(m_entries[index]);
}

int  ExampleClusterCollection::size() const {
  return m_entries.size();
}

ExampleCluster ExampleClusterCollection::create(){
  auto obj = new ExampleClusterObj();
  m_entries.emplace_back(obj);
  m_rel_Hits_tmp.push_back(obj->m_Hits);

  obj->id = {int(m_entries.size()-1),m_collectionID};
  return ExampleCluster(obj);
}

void ExampleClusterCollection::clear(){
  m_data->clear();
  for (auto& pointer : (*m_refCollections)) {pointer->clear(); }
  // clear relations to Hits. Make sure to unlink() the reference data as they may be gone already
  for (auto& pointer : m_rel_Hits_tmp) {for(auto& item : (*pointer)) {item.unlink();}; delete pointer;}
  m_rel_Hits_tmp.clear();
  for (auto& item : (*m_rel_Hits)) {item.unlink(); }
  m_rel_Hits->clear();

  for (auto& obj : m_entries) { delete obj; }
  m_entries.clear();
}

void ExampleClusterCollection::prepareForWrite(){
  int index = 0;
  auto size = m_entries.size();
  m_data->reserve(size);
  for (auto& obj : m_entries) {m_data->push_back(obj->data); }
  if (m_refCollections != nullptr) {
    for (auto& pointer : (*m_refCollections)) {pointer->clear(); }
  }
  
  for(int i=0, size = m_data->size(); i != size; ++i){
     (*m_data)[i].Hits_begin=index;
   (*m_data)[i].Hits_end+=index;
   index = (*m_data)[index].Hits_end;
   for(auto it : (*m_rel_Hits_tmp[i])) {
     if (it.getObjectID().index == podio::ObjectID::untracked)
       throw std::runtime_error("Trying to persistify untracked object");
     (*m_refCollections)[0]->emplace_back(it.getObjectID());
     m_rel_Hits->push_back(it);
   }

  }
  
}

void ExampleClusterCollection::prepareAfterRead(){
  int index = 0;
  for (auto& data : *m_data){
    auto obj = new ExampleClusterObj({index,m_collectionID}, data);
        obj->m_Hits = m_rel_Hits;
    m_entries.emplace_back(obj);
    ++index;
  }
}

bool ExampleClusterCollection::setReferences(const podio::ICollectionProvider* collectionProvider){
  for(unsigned int i=0, size=(*m_refCollections)[0]->size();i!=size;++i ) {
    auto id = (*(*m_refCollections)[0])[i];
    CollectionBase* coll = nullptr;
    collectionProvider->get(id.collectionID,coll);
    ExampleHitCollection* tmp_coll = static_cast<ExampleHitCollection*>(coll);
    auto tmp = (*tmp_coll)[id.index];
    m_rel_Hits->emplace_back(tmp);
  }


  return true; //TODO: check success
}

void ExampleClusterCollection::push_back(ConstExampleCluster object){
    int size = m_entries.size();
    auto obj = object.m_obj;
    if (obj->id.index == podio::ObjectID::untracked) {
        obj->id = {size,m_collectionID};
        m_entries.push_back(obj);
          m_rel_Hits_tmp.push_back(obj->m_Hits);

    } else {
      throw std::invalid_argument( "Object already in a collection. Cannot add it to a second collection " );

    }
}

void ExampleClusterCollection::setBuffer(void* address){
  m_data = static_cast<ExampleClusterDataContainer*>(address);
}


const ExampleCluster ExampleClusterCollectionIterator::operator* () const {
  m_object.m_obj = (*m_collection)[m_index];
  return m_object;
}

const ExampleCluster* ExampleClusterCollectionIterator::operator-> () const {
    m_object.m_obj = (*m_collection)[m_index];
    return &m_object;
}

const ExampleClusterCollectionIterator& ExampleClusterCollectionIterator::operator++() const {
  ++m_index;
 return *this;
}
