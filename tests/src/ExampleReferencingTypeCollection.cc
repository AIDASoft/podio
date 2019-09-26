// standard includes
#include <stdexcept>
#include "ExampleClusterCollection.h" 
#include "ExampleReferencingTypeCollection.h" 


#include "ExampleReferencingTypeCollection.h"



ExampleReferencingTypeCollection::ExampleReferencingTypeCollection() : m_isValid(false), m_collectionID(0), m_entries() , m_rel_Clusters(new std::vector<::ConstExampleCluster>()), m_rel_Refs(new std::vector<::ConstExampleReferencingType>()),m_data(new ExampleReferencingTypeDataContainer() ) {
    m_refCollections.push_back(new std::vector<podio::ObjectID>());
  m_refCollections.push_back(new std::vector<podio::ObjectID>());

}

ExampleReferencingTypeCollection::~ExampleReferencingTypeCollection() {
  clear();
  if (m_data != nullptr) delete m_data;
    for (auto& pointer : m_refCollections) { if (pointer != nullptr) delete pointer; }
  if (m_rel_Clusters != nullptr) { delete m_rel_Clusters; }
  if (m_rel_Refs != nullptr) { delete m_rel_Refs; }

}

const ExampleReferencingType ExampleReferencingTypeCollection::operator[](unsigned int index) const {
  return ExampleReferencingType(m_entries[index]);
}

const ExampleReferencingType ExampleReferencingTypeCollection::at(unsigned int index) const {
  return ExampleReferencingType(m_entries.at(index));
}

ExampleReferencingType ExampleReferencingTypeCollection::operator[](unsigned int index) {
  return ExampleReferencingType(m_entries[index]);
}

ExampleReferencingType ExampleReferencingTypeCollection::at(unsigned int index) {
  return ExampleReferencingType(m_entries.at(index));
}

int  ExampleReferencingTypeCollection::size() const {
  return m_entries.size();
}

ExampleReferencingType ExampleReferencingTypeCollection::create(){
  auto obj = new ExampleReferencingTypeObj();
  m_entries.emplace_back(obj);
  m_rel_Clusters_tmp.push_back(obj->m_Clusters);
  m_rel_Refs_tmp.push_back(obj->m_Refs);

  obj->id = {int(m_entries.size()-1),m_collectionID};
  return ExampleReferencingType(obj);
}

void ExampleReferencingTypeCollection::clear(){
  m_data->clear();
  for (auto& pointer : m_refCollections) { pointer->clear(); }
  // clear relations to Clusters. Make sure to unlink() the reference data s they may be gone already.
  for (auto& pointer : m_rel_Clusters_tmp) {
    for(auto& item : (*pointer)) {
      item.unlink();
    };
    delete pointer;
  }
  m_rel_Clusters_tmp.clear();
  for (auto& item : (*m_rel_Clusters)) { item.unlink(); }
  m_rel_Clusters->clear();
  // clear relations to Refs. Make sure to unlink() the reference data s they may be gone already.
  for (auto& pointer : m_rel_Refs_tmp) {
    for(auto& item : (*pointer)) {
      item.unlink();
    };
    delete pointer;
  }
  m_rel_Refs_tmp.clear();
  for (auto& item : (*m_rel_Refs)) { item.unlink(); }
  m_rel_Refs->clear();

  for (auto& obj : m_entries) { delete obj; }
  m_entries.clear();
}

void ExampleReferencingTypeCollection::prepareForWrite(){
  auto size = m_entries.size();
  m_data->reserve(size);
  for (auto& obj : m_entries) {m_data->push_back(obj->data); }
  for (auto& pointer : m_refCollections) {pointer->clear(); } 
  int Clusters_index =0;
  int Refs_index =0;

  for(int i=0, size = m_data->size(); i != size; ++i){
   (*m_data)[i].Clusters_begin=Clusters_index;
   (*m_data)[i].Clusters_end+=Clusters_index;
   Clusters_index = (*m_data)[i].Clusters_end;
   for(auto it : (*m_rel_Clusters_tmp[i])) {
     if (it.getObjectID().index == podio::ObjectID::untracked)
       throw std::runtime_error("Trying to persistify untracked object");
     m_refCollections[0]->emplace_back(it.getObjectID());
     m_rel_Clusters->push_back(it);
   }
   (*m_data)[i].Refs_begin=Refs_index;
   (*m_data)[i].Refs_end+=Refs_index;
   Refs_index = (*m_data)[i].Refs_end;
   for(auto it : (*m_rel_Refs_tmp[i])) {
     if (it.getObjectID().index == podio::ObjectID::untracked)
       throw std::runtime_error("Trying to persistify untracked object");
     m_refCollections[1]->emplace_back(it.getObjectID());
     m_rel_Refs->push_back(it);
   }

  }

}

void ExampleReferencingTypeCollection::prepareAfterRead(){
  int index = 0;
  for (auto& data : *m_data){
    auto obj = new ExampleReferencingTypeObj({index,m_collectionID}, data);
        obj->m_Clusters = m_rel_Clusters;   obj->m_Refs = m_rel_Refs;
    m_entries.emplace_back(obj);
    ++index;
  }
  m_isValid = true;
}

bool ExampleReferencingTypeCollection::setReferences(const podio::ICollectionProvider* collectionProvider){
  for(unsigned int i=0, size=m_refCollections[0]->size();i!=size;++i) {
    auto id = (*m_refCollections[0])[i];
    if (id.index != podio::ObjectID::invalid) {
      CollectionBase* coll = nullptr;
      collectionProvider->get(id.collectionID,coll);
      ExampleClusterCollection* tmp_coll = static_cast<ExampleClusterCollection*>(coll);
      auto tmp = (*tmp_coll)[id.index];
      m_rel_Clusters->emplace_back(tmp);
    } else {
      m_rel_Clusters->emplace_back(nullptr);
    }
  }
  for(unsigned int i=0, size=m_refCollections[1]->size();i!=size;++i) {
    auto id = (*m_refCollections[1])[i];
    if (id.index != podio::ObjectID::invalid) {
      CollectionBase* coll = nullptr;
      collectionProvider->get(id.collectionID,coll);
      ExampleReferencingTypeCollection* tmp_coll = static_cast<ExampleReferencingTypeCollection*>(coll);
      auto tmp = (*tmp_coll)[id.index];
      m_rel_Refs->emplace_back(tmp);
    } else {
      m_rel_Refs->emplace_back(nullptr);
    }
  }


  return true; //TODO: check success
}

void ExampleReferencingTypeCollection::push_back(ConstExampleReferencingType object){
  int size = m_entries.size();
  auto obj = object.m_obj;
  if (obj->id.index == podio::ObjectID::untracked) {
      obj->id = {size,m_collectionID};
      m_entries.push_back(obj);
        m_rel_Clusters_tmp.push_back(obj->m_Clusters);
  m_rel_Refs_tmp.push_back(obj->m_Refs);

  } else {
    throw std::invalid_argument( "Object already in a collection. Cannot add it to a second collection " );
  }
}

void ExampleReferencingTypeCollection::setBuffer(void* address){
  if (m_data != nullptr) delete m_data;
  m_data = static_cast<ExampleReferencingTypeDataContainer*>(address);
}


const ExampleReferencingType ExampleReferencingTypeCollectionIterator::operator* () const {
  m_object.m_obj = (*m_collection)[m_index];
  return m_object;
}

const ExampleReferencingType* ExampleReferencingTypeCollectionIterator::operator-> () const {
  m_object.m_obj = (*m_collection)[m_index];
  return &m_object;
}

const ExampleReferencingTypeCollectionIterator& ExampleReferencingTypeCollectionIterator::operator++() const {
  ++m_index;
  return *this;
}

std::ostream& operator<<( std::ostream& o,const ExampleReferencingTypeCollection& v){
  std::ios::fmtflags old_flags = o.flags() ; 
  o << "id:          " << std::endl ;
   for(int i = 0; i < v.size(); i++){
     o << std::scientific << std::showpos  << std::setw(12)  << v[i].id() << " "   << std::endl;
  o << "     Clusters : " ;
  for(unsigned j=0,N=v[i].Clusters_size(); j<N ; ++j)
    o << v[i].Clusters(j).id() << " " ; 
  o << std::endl ;
  o << "     Refs : " ;
  for(unsigned j=0,N=v[i].Refs_size(); j<N ; ++j)
    o << v[i].Refs(j).id() << " " ; 
  o << std::endl ;
  }
o.flags(old_flags);
  return o ;
}



