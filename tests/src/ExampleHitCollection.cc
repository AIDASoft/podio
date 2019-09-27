// standard includes
#include <stdexcept>


#include "ExampleHitCollection.h"



ExampleHitCollection::ExampleHitCollection() : m_isValid(false), m_collectionID(0), m_entries() ,m_data(new ExampleHitDataContainer() ) {
  
}

ExampleHitCollection::~ExampleHitCollection() {
  clear();
  if (m_data != nullptr) delete m_data;
  
}

const ExampleHit ExampleHitCollection::operator[](unsigned int index) const {
  return ExampleHit(m_entries[index]);
}

const ExampleHit ExampleHitCollection::at(unsigned int index) const {
  return ExampleHit(m_entries.at(index));
}

ExampleHit ExampleHitCollection::operator[](unsigned int index) {
  return ExampleHit(m_entries[index]);
}

ExampleHit ExampleHitCollection::at(unsigned int index) {
  return ExampleHit(m_entries.at(index));
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
  auto size = m_entries.size();
  m_data->reserve(size);
  for (auto& obj : m_entries) {m_data->push_back(obj->data); }
  for (auto& pointer : m_refCollections) {pointer->clear(); } 

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
  m_isValid = true;
}

bool ExampleHitCollection::setReferences(const podio::ICollectionProvider* collectionProvider){


  return true; //TODO: check success
}

void ExampleHitCollection::push_back(ConstExampleHit object){
  int size = m_entries.size();
  auto obj = object.m_obj;
  if (obj->id.index == podio::ObjectID::untracked) {
      obj->id = {size,m_collectionID};
      m_entries.push_back(obj);
      
  } else {
    throw std::invalid_argument( "Object already in a collection. Cannot add it to a second collection " );
  }
}

void ExampleHitCollection::setBuffer(void* address){
  if (m_data != nullptr) delete m_data;
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

std::ostream& operator<<( std::ostream& o,const ExampleHitCollection& v){
  std::ios::fmtflags old_flags = o.flags() ; 
  o << "id:          cellID:       x:            y:            z:            energy:       " << std::endl ;
   for(int i = 0; i < v.size(); i++){
     o << std::scientific << std::showpos  << std::setw(12)  << v[i].id() << " "  << std::setw(12) << v[i].cellID() << " " << std::setw(12) << v[i].x() << " " << std::setw(12) << v[i].y() << " " << std::setw(12) << v[i].z() << " " << std::setw(12) << v[i].energy() << " "  << std::endl;
  }
o.flags(old_flags);
  return o ;
}



