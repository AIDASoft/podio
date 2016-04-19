// standard includes
#include <stdexcept>
#include "ExampleHitCollection.h" 
#include "ExampleClusterCollection.h" 


#include "ExampleClusterCollection.h"



ExampleClusterCollection::ExampleClusterCollection() : m_isValid(false), m_collectionID(0), m_entries() , m_rel_Hits(new std::vector<::ConstExampleHit>()), m_rel_Clusters(new std::vector<::ConstExampleCluster>()),m_data(new ExampleClusterDataContainer() ) {
    m_refCollections.push_back(new std::vector<podio::ObjectID>());
  m_refCollections.push_back(new std::vector<podio::ObjectID>());

}

ExampleClusterCollection::~ExampleClusterCollection() {
  clear();
  if (m_data != nullptr) delete m_data;
    for (auto& pointer : m_refCollections) { if (pointer != nullptr) delete pointer;}

};

const ExampleCluster ExampleClusterCollection::operator[](unsigned int index) const {
  return ExampleCluster(m_entries[index]);
}

const ExampleCluster ExampleClusterCollection::at(unsigned int index) const {
  return ExampleCluster(m_entries.at(index));
}

int  ExampleClusterCollection::size() const {
  return m_entries.size();
}

ExampleCluster ExampleClusterCollection::create(){
  auto obj = new ExampleClusterObj();
  m_entries.emplace_back(obj);
  m_rel_Hits_tmp.push_back(obj->m_Hits);
  m_rel_Clusters_tmp.push_back(obj->m_Clusters);

  obj->id = {int(m_entries.size()-1),m_collectionID};
  return ExampleCluster(obj);
}

void ExampleClusterCollection::clear(){
  m_data->clear();
  for (auto& pointer : m_refCollections) { pointer->clear(); }
  // clear relations to Hits. Make sure to unlink() the reference data s they may be gone already.
  for (auto& pointer : m_rel_Hits_tmp) {
    for(auto& item : (*pointer)) {
      item.unlink();
    };
    delete pointer;
  }
  m_rel_Hits_tmp.clear();
  // clear relations to Clusters. Make sure to unlink() the reference data s they may be gone already.
  for (auto& pointer : m_rel_Clusters_tmp) {
    for(auto& item : (*pointer)) {
      item.unlink();
    };
    delete pointer;
  }
  m_rel_Clusters_tmp.clear();

  for (auto& obj : m_entries) { delete obj; }
  m_entries.clear();
}

void ExampleClusterCollection::prepareForWrite(){
  auto size = m_entries.size();
  m_data->reserve(size);
  for (auto& obj : m_entries) {m_data->push_back(obj->data); }
  for (auto& pointer : m_refCollections) {pointer->clear(); } 
  int Hits_index =0;
  int Clusters_index =0;

  for(int i=0, size = m_data->size(); i != size; ++i){
   (*m_data)[i].Hits_begin=Hits_index;
   (*m_data)[i].Hits_end+=Hits_index;
   Hits_index = (*m_data)[Hits_index].Hits_end;
   for(auto it : (*m_rel_Hits_tmp[i])) {
     if (it.getObjectID().index == podio::ObjectID::untracked)
       throw std::runtime_error("Trying to persistify untracked object");
     m_refCollections[0]->emplace_back(it.getObjectID());
     m_rel_Hits->push_back(it);
   }
   (*m_data)[i].Clusters_begin=Clusters_index;
   (*m_data)[i].Clusters_end+=Clusters_index;
   Clusters_index = (*m_data)[Clusters_index].Clusters_end;
   for(auto it : (*m_rel_Clusters_tmp[i])) {
     if (it.getObjectID().index == podio::ObjectID::untracked)
       throw std::runtime_error("Trying to persistify untracked object");
     m_refCollections[1]->emplace_back(it.getObjectID());
     m_rel_Clusters->push_back(it);
   }

  }

}

void ExampleClusterCollection::prepareAfterRead(){
  int index = 0;
  for (auto& data : *m_data){
    auto obj = new ExampleClusterObj({index,m_collectionID}, data);
        obj->m_Hits = m_rel_Hits;   obj->m_Clusters = m_rel_Clusters;
    m_entries.emplace_back(obj);
    ++index;
  }
  m_isValid = true;  
}

bool ExampleClusterCollection::setReferences(const podio::ICollectionProvider* collectionProvider){
  for(unsigned int i=0, size=m_refCollections[0]->size();i!=size;++i) {
    auto id = (*m_refCollections[0])[i];
    CollectionBase* coll = nullptr;
    collectionProvider->get(id.collectionID,coll);
    ExampleHitCollection* tmp_coll = static_cast<ExampleHitCollection*>(coll);
    auto tmp = (*tmp_coll)[id.index];
    m_rel_Hits->emplace_back(tmp);
  }
  for(unsigned int i=0, size=m_refCollections[1]->size();i!=size;++i) {
    auto id = (*m_refCollections[1])[i];
    CollectionBase* coll = nullptr;
    collectionProvider->get(id.collectionID,coll);
    ExampleClusterCollection* tmp_coll = static_cast<ExampleClusterCollection*>(coll);
    auto tmp = (*tmp_coll)[id.index];
    m_rel_Clusters->emplace_back(tmp);
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
  m_rel_Clusters_tmp.push_back(obj->m_Clusters);

  } else {
    throw std::invalid_argument( "Object already in a collection. Cannot add it to a second collection " );
  }
}

void ExampleClusterCollection::setBuffer(void* address){
  if (m_data != nullptr) delete m_data;
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


