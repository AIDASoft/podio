// standard includes
#include <stdexcept>


#include "ExampleHitCollection.h"

ExampleHitCollection::ExampleHitCollection() : m_collectionID(0), m_entries() ,m_refCollections(nullptr), m_data(new ExampleHitDataContainer() ) {
  
}

const ExampleHit ExampleHitCollection::operator[](int index) const {
  return ExampleHit(m_entries[index]);
}

int  ExampleHitCollection::size() const {
  return m_entries.size();
}

ExampleHit ExampleHitCollection::create(){
  auto obj = new ExampleHitObj();
  m_entries.emplace_back(obj);

  obj->id = {int(m_entries.size()-1),m_collectionID};
  return ExampleHit(obj);
}

void ExampleHitCollection::clear(){
  m_data->clear();

  for (auto& obj : m_entries) { delete obj; }
  m_entries.clear();
}

void ExampleHitCollection::prepareForWrite(){
  int index = 0;
  auto size = m_entries.size();
  m_data->reserve(size);
  for (auto& obj : m_entries) {m_data->push_back(obj->data); }
  if (m_refCollections != nullptr) {
    for (auto& pointer : (*m_refCollections)) {pointer->clear(); }
  }
  
  for(int i=0, size = m_data->size(); i != size; ++i){
  
  }

}

void ExampleHitCollection::prepareAfterRead(){
  int index = 0;
  for (auto& data : *m_data){
    auto obj = new ExampleHitObj({index,m_collectionID}, data);
    
    m_entries.emplace_back(obj);
    ++index;
  }
}

bool ExampleHitCollection::setReferences(const albers::ICollectionProvider* collectionProvider){

  return true; //TODO: check success
}

void ExampleHitCollection::push_back(ExampleHit object){
    int size = m_entries.size();
    auto obj = object.m_obj;
    if (obj->id.index == albers::ObjectID::untracked) {
        obj->id = {size,m_collectionID};
        m_entries.push_back(obj);
        
    } else {
      throw std::invalid_argument( "Cannot add an object to collection that is already owned by another collection." );

    }
}

void ExampleHitCollection::setBuffer(void* address){
  m_data = static_cast<ExampleHitDataContainer*>(address);
}


const ExampleHit ExampleHitCollectionIterator::operator* () const {
  m_object.m_obj = (*m_collection)[m_index];
  return m_object;
}

const ExampleHit* ExampleHitCollectionIterator::operator-> () const {
    m_object.m_obj = (*m_collection)[m_index];
    return &m_object;
}

const ExampleHitCollectionIterator& ExampleHitCollectionIterator::operator++() const {
  ++m_index;
 return *this;
}
