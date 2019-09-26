// standard includes
#include <stdexcept>


#include "ExampleWithArrayCollection.h"



ExampleWithArrayCollection::ExampleWithArrayCollection() : m_isValid(false), m_collectionID(0), m_entries() ,m_data(new ExampleWithArrayDataContainer() ) {
  
}

ExampleWithArrayCollection::~ExampleWithArrayCollection() {
  clear();
  if (m_data != nullptr) delete m_data;
  
}

const ExampleWithArray ExampleWithArrayCollection::operator[](unsigned int index) const {
  return ExampleWithArray(m_entries[index]);
}

const ExampleWithArray ExampleWithArrayCollection::at(unsigned int index) const {
  return ExampleWithArray(m_entries.at(index));
}

ExampleWithArray ExampleWithArrayCollection::operator[](unsigned int index) {
  return ExampleWithArray(m_entries[index]);
}

ExampleWithArray ExampleWithArrayCollection::at(unsigned int index) {
  return ExampleWithArray(m_entries.at(index));
}

int  ExampleWithArrayCollection::size() const {
  return m_entries.size();
}

ExampleWithArray ExampleWithArrayCollection::create(){
  auto obj = new ExampleWithArrayObj();
  m_entries.emplace_back(obj);

  obj->id = {int(m_entries.size()-1),m_collectionID};
  return ExampleWithArray(obj);
}

void ExampleWithArrayCollection::clear(){
  m_data->clear();

  for (auto& obj : m_entries) { delete obj; }
  m_entries.clear();
}

void ExampleWithArrayCollection::prepareForWrite(){
  auto size = m_entries.size();
  m_data->reserve(size);
  for (auto& obj : m_entries) {m_data->push_back(obj->data); }
  for (auto& pointer : m_refCollections) {pointer->clear(); } 

  for(int i=0, size = m_data->size(); i != size; ++i){

  }

}

void ExampleWithArrayCollection::prepareAfterRead(){
  int index = 0;
  for (auto& data : *m_data){
    auto obj = new ExampleWithArrayObj({index,m_collectionID}, data);
    
    m_entries.emplace_back(obj);
    ++index;
  }
  m_isValid = true;
}

bool ExampleWithArrayCollection::setReferences(const podio::ICollectionProvider* collectionProvider){


  return true; //TODO: check success
}

void ExampleWithArrayCollection::push_back(ConstExampleWithArray object){
  int size = m_entries.size();
  auto obj = object.m_obj;
  if (obj->id.index == podio::ObjectID::untracked) {
      obj->id = {size,m_collectionID};
      m_entries.push_back(obj);
      
  } else {
    throw std::invalid_argument( "Object already in a collection. Cannot add it to a second collection " );
  }
}

void ExampleWithArrayCollection::setBuffer(void* address){
  if (m_data != nullptr) delete m_data;
  m_data = static_cast<ExampleWithArrayDataContainer*>(address);
}


const ExampleWithArray ExampleWithArrayCollectionIterator::operator* () const {
  m_object.m_obj = (*m_collection)[m_index];
  return m_object;
}

const ExampleWithArray* ExampleWithArrayCollectionIterator::operator-> () const {
  m_object.m_obj = (*m_collection)[m_index];
  return &m_object;
}

const ExampleWithArrayCollectionIterator& ExampleWithArrayCollectionIterator::operator++() const {
  ++m_index;
  return *this;
}

std::ostream& operator<<( std::ostream& o,const ExampleWithArrayCollection& v){
  std::ios::fmtflags old_flags = o.flags() ; 
  o << "id:          arrayStruct [data,]:myArray:      anotherArray: snail_case_a: snail_case_A: structArray:  " << std::endl ;
   for(int i = 0; i < v.size(); i++){
     o << std::scientific << std::showpos  << std::setw(12)  << v[i].id() << " "  << std::setw(12) << v[i].arrayStruct() << " "  << std::endl;
  }
o.flags(old_flags);
  return o ;
}



