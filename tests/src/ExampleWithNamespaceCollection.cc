// standard includes
#include <stdexcept>


#include "ExampleWithNamespaceCollection.h"

namespace ex42 {

ExampleWithNamespaceCollection::ExampleWithNamespaceCollection() : m_isValid(false), m_collectionID(0), m_entries() ,m_data(new ExampleWithNamespaceDataContainer() ) {
  
}

ExampleWithNamespaceCollection::~ExampleWithNamespaceCollection() {
  clear();
  if (m_data != nullptr) delete m_data;
  
}

const ExampleWithNamespace ExampleWithNamespaceCollection::operator[](unsigned int index) const {
  return ExampleWithNamespace(m_entries[index]);
}

const ExampleWithNamespace ExampleWithNamespaceCollection::at(unsigned int index) const {
  return ExampleWithNamespace(m_entries.at(index));
}

ExampleWithNamespace ExampleWithNamespaceCollection::operator[](unsigned int index) {
  return ExampleWithNamespace(m_entries[index]);
}

ExampleWithNamespace ExampleWithNamespaceCollection::at(unsigned int index) {
  return ExampleWithNamespace(m_entries.at(index));
}

int  ExampleWithNamespaceCollection::size() const {
  return m_entries.size();
}

ExampleWithNamespace ExampleWithNamespaceCollection::create(){
  auto obj = new ExampleWithNamespaceObj();
  m_entries.emplace_back(obj);

  obj->id = {int(m_entries.size()-1),m_collectionID};
  return ExampleWithNamespace(obj);
}

void ExampleWithNamespaceCollection::clear(){
  m_data->clear();

  for (auto& obj : m_entries) { delete obj; }
  m_entries.clear();
}

void ExampleWithNamespaceCollection::prepareForWrite(){
  auto size = m_entries.size();
  m_data->reserve(size);
  for (auto& obj : m_entries) {m_data->push_back(obj->data); }
  for (auto& pointer : m_refCollections) {pointer->clear(); } 

  for(int i=0, size = m_data->size(); i != size; ++i){

  }

}

void ExampleWithNamespaceCollection::prepareAfterRead(){
  int index = 0;
  for (auto& data : *m_data){
    auto obj = new ExampleWithNamespaceObj({index,m_collectionID}, data);
    
    m_entries.emplace_back(obj);
    ++index;
  }
  m_isValid = true;
}

bool ExampleWithNamespaceCollection::setReferences(const podio::ICollectionProvider* collectionProvider){


  return true; //TODO: check success
}

void ExampleWithNamespaceCollection::push_back(ConstExampleWithNamespace object){
  int size = m_entries.size();
  auto obj = object.m_obj;
  if (obj->id.index == podio::ObjectID::untracked) {
      obj->id = {size,m_collectionID};
      m_entries.push_back(obj);
      
  } else {
    throw std::invalid_argument( "Object already in a collection. Cannot add it to a second collection " );
  }
}

void ExampleWithNamespaceCollection::setBuffer(void* address){
  if (m_data != nullptr) delete m_data;
  m_data = static_cast<ExampleWithNamespaceDataContainer*>(address);
}


const ExampleWithNamespace ExampleWithNamespaceCollectionIterator::operator* () const {
  m_object.m_obj = (*m_collection)[m_index];
  return m_object;
}

const ExampleWithNamespace* ExampleWithNamespaceCollectionIterator::operator-> () const {
  m_object.m_obj = (*m_collection)[m_index];
  return &m_object;
}

const ExampleWithNamespaceCollectionIterator& ExampleWithNamespaceCollectionIterator::operator++() const {
  ++m_index;
  return *this;
}

std::ostream& operator<<( std::ostream& o,const ExampleWithNamespaceCollection& v){
  std::ios::fmtflags old_flags = o.flags() ; 
  o << "id:          data [y,x,]:                " << std::endl ;
   for(int i = 0; i < v.size(); i++){
     o << std::scientific << std::showpos  << std::setw(12)  << v[i].id() << " "  << std::setw(12) << v[i].data() << " "  << std::endl;
  }
o.flags(old_flags);
  return o ;
}


} // namespace ex42
