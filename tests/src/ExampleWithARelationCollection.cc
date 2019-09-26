// standard includes
#include <stdexcept>
#include "ExampleWithNamespaceCollection.h" 
#include "ExampleWithNamespaceCollection.h"


#include "ExampleWithARelationCollection.h"

namespace ex42 {

ExampleWithARelationCollection::ExampleWithARelationCollection() : m_isValid(false), m_collectionID(0), m_entries() , m_rel_refs(new std::vector<ex42::ConstExampleWithNamespace>()), m_rel_ref(new std::vector<ex42::ConstExampleWithNamespace>()),m_data(new ExampleWithARelationDataContainer() ) {
    m_refCollections.push_back(new std::vector<podio::ObjectID>());
  m_refCollections.push_back(new std::vector<podio::ObjectID>());

}

ExampleWithARelationCollection::~ExampleWithARelationCollection() {
  clear();
  if (m_data != nullptr) delete m_data;
    for (auto& pointer : m_refCollections) { if (pointer != nullptr) delete pointer; }
  if (m_rel_refs != nullptr) { delete m_rel_refs; }
  if (m_rel_ref != nullptr) { delete m_rel_ref; }

}

const ExampleWithARelation ExampleWithARelationCollection::operator[](unsigned int index) const {
  return ExampleWithARelation(m_entries[index]);
}

const ExampleWithARelation ExampleWithARelationCollection::at(unsigned int index) const {
  return ExampleWithARelation(m_entries.at(index));
}

ExampleWithARelation ExampleWithARelationCollection::operator[](unsigned int index) {
  return ExampleWithARelation(m_entries[index]);
}

ExampleWithARelation ExampleWithARelationCollection::at(unsigned int index) {
  return ExampleWithARelation(m_entries.at(index));
}

int  ExampleWithARelationCollection::size() const {
  return m_entries.size();
}

ExampleWithARelation ExampleWithARelationCollection::create(){
  auto obj = new ExampleWithARelationObj();
  m_entries.emplace_back(obj);
  m_rel_refs_tmp.push_back(obj->m_refs);

  obj->id = {int(m_entries.size()-1),m_collectionID};
  return ExampleWithARelation(obj);
}

void ExampleWithARelationCollection::clear(){
  m_data->clear();
  for (auto& pointer : m_refCollections) { pointer->clear(); }
  // clear relations to refs. Make sure to unlink() the reference data s they may be gone already.
  for (auto& pointer : m_rel_refs_tmp) {
    for(auto& item : (*pointer)) {
      item.unlink();
    };
    delete pointer;
  }
  m_rel_refs_tmp.clear();
  for (auto& item : (*m_rel_refs)) { item.unlink(); }
  m_rel_refs->clear();
  for (auto& item : (*m_rel_ref)) { item.unlink(); }
  m_rel_ref->clear();

  for (auto& obj : m_entries) { delete obj; }
  m_entries.clear();
}

void ExampleWithARelationCollection::prepareForWrite(){
  auto size = m_entries.size();
  m_data->reserve(size);
  for (auto& obj : m_entries) {m_data->push_back(obj->data); }
  for (auto& pointer : m_refCollections) {pointer->clear(); } 
  int refs_index =0;

  for(int i=0, size = m_data->size(); i != size; ++i){
   (*m_data)[i].refs_begin=refs_index;
   (*m_data)[i].refs_end+=refs_index;
   refs_index = (*m_data)[i].refs_end;
   for(auto it : (*m_rel_refs_tmp[i])) {
     if (it.getObjectID().index == podio::ObjectID::untracked)
       throw std::runtime_error("Trying to persistify untracked object");
     m_refCollections[0]->emplace_back(it.getObjectID());
     m_rel_refs->push_back(it);
   }

  }
  for (auto& obj : m_entries) {
    if (obj->m_ref != nullptr) {
      m_refCollections[1]->emplace_back(obj->m_ref->getObjectID());
    } else {
      m_refCollections[1]->push_back({-2,-2});
    }
  }

}

void ExampleWithARelationCollection::prepareAfterRead(){
  int index = 0;
  for (auto& data : *m_data){
    auto obj = new ExampleWithARelationObj({index,m_collectionID}, data);
        obj->m_refs = m_rel_refs;
    m_entries.emplace_back(obj);
    ++index;
  }
  m_isValid = true;
}

bool ExampleWithARelationCollection::setReferences(const podio::ICollectionProvider* collectionProvider){
  for(unsigned int i=0, size=m_refCollections[0]->size();i!=size;++i) {
    auto id = (*m_refCollections[0])[i];
    if (id.index != podio::ObjectID::invalid) {
      CollectionBase* coll = nullptr;
      collectionProvider->get(id.collectionID,coll);
      ex42::ExampleWithNamespaceCollection* tmp_coll = static_cast<ex42::ExampleWithNamespaceCollection*>(coll);
      auto tmp = (*tmp_coll)[id.index];
      m_rel_refs->emplace_back(tmp);
    } else {
      m_rel_refs->emplace_back(nullptr);
    }
  }

  for(unsigned int i = 0, size = m_entries.size(); i != size; ++i) {
    auto id = (*m_refCollections[1])[i];
    if (id.index != podio::ObjectID::invalid) {
      CollectionBase* coll = nullptr;
      collectionProvider->get(id.collectionID,coll);
      ex42::ExampleWithNamespaceCollection* tmp_coll = static_cast<ex42::ExampleWithNamespaceCollection*>(coll);
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
        m_rel_refs_tmp.push_back(obj->m_refs);

  } else {
    throw std::invalid_argument( "Object already in a collection. Cannot add it to a second collection " );
  }
}

void ExampleWithARelationCollection::setBuffer(void* address){
  if (m_data != nullptr) delete m_data;
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

std::ostream& operator<<( std::ostream& o,const ExampleWithARelationCollection& v){
  std::ios::fmtflags old_flags = o.flags() ; 
  o << "id:          number:       " << std::endl ;
   for(int i = 0; i < v.size(); i++){
     o << std::scientific << std::showpos  << std::setw(12)  << v[i].id() << " "  << std::setw(12) << v[i].number() << " "  << std::endl;
  o << "     refs : " ;
  for(unsigned j=0,N=v[i].refs_size(); j<N ; ++j)
    o << v[i].refs(j).id() << " " ; 
  o << std::endl ;
  o << "     ref : " ;
  o << v[i].ref().id() << std::endl;
  }
o.flags(old_flags);
  return o ;
}


} // namespace ex42
