// standard includes
#include <stdexcept>
#include "ExampleClusterCollection.h"


#include "ExampleWithOneRelationCollection.h"



ExampleWithOneRelationCollection::ExampleWithOneRelationCollection() : m_isValid(false), m_collectionID(0), m_entries() , m_rel_cluster(new std::vector<::ConstExampleCluster>()),m_data(new ExampleWithOneRelationDataContainer() ) {
    m_refCollections.push_back(new std::vector<podio::ObjectID>());

}

ExampleWithOneRelationCollection::~ExampleWithOneRelationCollection() {
  clear();
  if (m_data != nullptr) delete m_data;
    for (auto& pointer : m_refCollections) { if (pointer != nullptr) delete pointer; }
  if (m_rel_cluster != nullptr) { delete m_rel_cluster; }

}

const ExampleWithOneRelation ExampleWithOneRelationCollection::operator[](unsigned int index) const {
  return ExampleWithOneRelation(m_entries[index]);
}

const ExampleWithOneRelation ExampleWithOneRelationCollection::at(unsigned int index) const {
  return ExampleWithOneRelation(m_entries.at(index));
}

ExampleWithOneRelation ExampleWithOneRelationCollection::operator[](unsigned int index) {
  return ExampleWithOneRelation(m_entries[index]);
}

ExampleWithOneRelation ExampleWithOneRelationCollection::at(unsigned int index) {
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
  for (auto& pointer : m_refCollections) { pointer->clear(); }
  for (auto& item : (*m_rel_cluster)) { item.unlink(); }
  m_rel_cluster->clear();

  for (auto& obj : m_entries) { delete obj; }
  m_entries.clear();
}

void ExampleWithOneRelationCollection::prepareForWrite(){
  auto size = m_entries.size();
  m_data->reserve(size);
  for (auto& obj : m_entries) {m_data->push_back(obj->data); }
  for (auto& pointer : m_refCollections) {pointer->clear(); } 

  for(int i=0, size = m_data->size(); i != size; ++i){

  }
  for (auto& obj : m_entries) {
    if (obj->m_cluster != nullptr) {
      m_refCollections[0]->emplace_back(obj->m_cluster->getObjectID());
    } else {
      m_refCollections[0]->push_back({-2,-2});
    }
  }

}

void ExampleWithOneRelationCollection::prepareAfterRead(){
  int index = 0;
  for (auto& data : *m_data){
    auto obj = new ExampleWithOneRelationObj({index,m_collectionID}, data);
    
    m_entries.emplace_back(obj);
    ++index;
  }
  m_isValid = true;
}

bool ExampleWithOneRelationCollection::setReferences(const podio::ICollectionProvider* collectionProvider){

  for(unsigned int i = 0, size = m_entries.size(); i != size; ++i) {
    auto id = (*m_refCollections[0])[i];
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
  if (m_data != nullptr) delete m_data;
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

std::ostream& operator<<( std::ostream& o,const ExampleWithOneRelationCollection& v){
  std::ios::fmtflags old_flags = o.flags() ; 
  o << "id:          " << std::endl ;
   for(int i = 0; i < v.size(); i++){
     o << std::scientific << std::showpos  << std::setw(12)  << v[i].id() << " "   << std::endl;
  o << "     cluster : " ;
  o << v[i].cluster().id() << std::endl;
  }
o.flags(old_flags);
  return o ;
}



