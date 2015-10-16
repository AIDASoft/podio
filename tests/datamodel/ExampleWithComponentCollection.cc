// standard includes
#include <stdexcept>


#include "ExampleWithComponentCollection.h"

ExampleWithComponentCollection::ExampleWithComponentCollection() : m_collectionID(0), m_entries() ,m_refCollections(nullptr), m_data(new ExampleWithComponentDataContainer() ) {
  
}

const ExampleWithComponent ExampleWithComponentCollection::operator[](unsigned int index) const {
  return ExampleWithComponent(m_entries[index]);
}

const ExampleWithComponent ExampleWithComponentCollection::at(unsigned int index) const {
  return ExampleWithComponent(m_entries.at(index));
}

int  ExampleWithComponentCollection::size() const {
  return m_entries.size();
}

ExampleWithComponent ExampleWithComponentCollection::create(){
  auto obj = new ExampleWithComponentObj();
  m_entries.emplace_back(obj);

  obj->id = {int(m_entries.size()-1),m_collectionID};
  return ExampleWithComponent(obj);
}

void ExampleWithComponentCollection::clear(){
  m_data->clear();

  for (auto& obj : m_entries) { delete obj; }
  m_entries.clear();
}

void ExampleWithComponentCollection::prepareForWrite(){
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

void ExampleWithComponentCollection::prepareAfterRead(){
  int index = 0;
  for (auto& data : *m_data){
    auto obj = new ExampleWithComponentObj({index,m_collectionID}, data);
    
    m_entries.emplace_back(obj);
    ++index;
  }
}

bool ExampleWithComponentCollection::setReferences(const podio::ICollectionProvider* collectionProvider){


  return true; //TODO: check success
}

void ExampleWithComponentCollection::push_back(ConstExampleWithComponent object){
    int size = m_entries.size();
    auto obj = object.m_obj;
    if (obj->id.index == podio::ObjectID::untracked) {
        obj->id = {size,m_collectionID};
        m_entries.push_back(obj);
        
    } else {
      throw std::invalid_argument( "Object already in a collection. Cannot add it to a second collection " );

    }
}

void ExampleWithComponentCollection::setBuffer(void* address){
  m_data = static_cast<ExampleWithComponentDataContainer*>(address);
}


const ExampleWithComponent ExampleWithComponentCollectionIterator::operator* () const {
  m_object.m_obj = (*m_collection)[m_index];
  return m_object;
}

const ExampleWithComponent* ExampleWithComponentCollectionIterator::operator-> () const {
    m_object.m_obj = (*m_collection)[m_index];
    return &m_object;
}

const ExampleWithComponentCollectionIterator& ExampleWithComponentCollectionIterator::operator++() const {
  ++m_index;
 return *this;
}
