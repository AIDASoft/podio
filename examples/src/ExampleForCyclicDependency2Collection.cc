// standard includes
#include <stdexcept>
#include "ExampleForCyclicDependency1Collection.h" 


#include "ExampleForCyclicDependency2Collection.h"



ExampleForCyclicDependency2Collection::ExampleForCyclicDependency2Collection() : m_collectionID(0), m_entries() ,m_rel_ref(new std::vector<ConstExampleForCyclicDependency1>()),m_refCollections(nullptr), m_data(new ExampleForCyclicDependency2DataContainer() ) {
    m_refCollections = new podio::CollRefCollection();
  m_refCollections->push_back(new std::vector<podio::ObjectID>());

}

const ExampleForCyclicDependency2 ExampleForCyclicDependency2Collection::operator[](unsigned int index) const {
  return ExampleForCyclicDependency2(m_entries[index]);
}

const ExampleForCyclicDependency2 ExampleForCyclicDependency2Collection::at(unsigned int index) const {
  return ExampleForCyclicDependency2(m_entries.at(index));
}

int  ExampleForCyclicDependency2Collection::size() const {
  return m_entries.size();
}

ExampleForCyclicDependency2 ExampleForCyclicDependency2Collection::create(){
  auto obj = new ExampleForCyclicDependency2Obj();
  m_entries.emplace_back(obj);

  obj->id = {int(m_entries.size()-1),m_collectionID};
  return ExampleForCyclicDependency2(obj);
}

void ExampleForCyclicDependency2Collection::clear(){
  m_data->clear();
  for (auto& pointer : (*m_refCollections)) {pointer->clear(); }
  for (auto& item : (*m_rel_ref)) {item.unlink(); }
  m_rel_ref->clear();

  for (auto& obj : m_entries) { delete obj; }
  m_entries.clear();
}

void ExampleForCyclicDependency2Collection::prepareForWrite(){
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

void ExampleForCyclicDependency2Collection::prepareAfterRead(){
  int index = 0;
  for (auto& data : *m_data){
    auto obj = new ExampleForCyclicDependency2Obj({index,m_collectionID}, data);
    
    m_entries.emplace_back(obj);
    ++index;
  }
}

bool ExampleForCyclicDependency2Collection::setReferences(const podio::ICollectionProvider* collectionProvider){

  for(unsigned int i=0, size=m_entries.size();i!=size;++i ) {
    auto id = (*(*m_refCollections)[0])[i];
    if (id.index != podio::ObjectID::invalid) {
      CollectionBase* coll = nullptr;
      collectionProvider->get(id.collectionID,coll);
      ExampleForCyclicDependency1Collection* tmp_coll = static_cast<ExampleForCyclicDependency1Collection*>(coll);
      m_entries[i]->m_ref = new ConstExampleForCyclicDependency1((*tmp_coll)[id.index]);
    } else {
      m_entries[i]->m_ref = nullptr;
    }
  }

  return true; //TODO: check success
}

void ExampleForCyclicDependency2Collection::push_back(ConstExampleForCyclicDependency2 object){
    int size = m_entries.size();
    auto obj = object.m_obj;
    if (obj->id.index == podio::ObjectID::untracked) {
        obj->id = {size,m_collectionID};
        m_entries.push_back(obj);
        
    } else {
      throw std::invalid_argument( "Object already in a collection. Cannot add it to a second collection " );

    }
}

void ExampleForCyclicDependency2Collection::setBuffer(void* address){
  m_data = static_cast<ExampleForCyclicDependency2DataContainer*>(address);
}


const ExampleForCyclicDependency2 ExampleForCyclicDependency2CollectionIterator::operator* () const {
  m_object.m_obj = (*m_collection)[m_index];
  return m_object;
}

const ExampleForCyclicDependency2* ExampleForCyclicDependency2CollectionIterator::operator-> () const {
    m_object.m_obj = (*m_collection)[m_index];
    return &m_object;
}

const ExampleForCyclicDependency2CollectionIterator& ExampleForCyclicDependency2CollectionIterator::operator++() const {
  ++m_index;
 return *this;
}


