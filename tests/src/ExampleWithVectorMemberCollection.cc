// standard includes
#include <stdexcept>
#include <numeric>


#include "ExampleWithVectorMemberCollection.h"



ExampleWithVectorMemberCollection::ExampleWithVectorMemberCollection() : m_isValid(false), m_collectionID(0), m_entries() ,m_data(new ExampleWithVectorMemberDataContainer() ) {
    m_vecmem_info.push_back( std::make_pair( "int", &m_vec_count )) ; 
  m_vec_count = new std::vector<int>() ;

}

ExampleWithVectorMemberCollection::~ExampleWithVectorMemberCollection() {
  clear();
  if (m_data != nullptr) delete m_data;
    if(m_vec_count != nullptr) delete m_vec_count;

}

const ExampleWithVectorMember ExampleWithVectorMemberCollection::operator[](unsigned int index) const {
  return ExampleWithVectorMember(m_entries[index]);
}

const ExampleWithVectorMember ExampleWithVectorMemberCollection::at(unsigned int index) const {
  return ExampleWithVectorMember(m_entries.at(index));
}

ExampleWithVectorMember ExampleWithVectorMemberCollection::operator[](unsigned int index) {
  return ExampleWithVectorMember(m_entries[index]);
}

ExampleWithVectorMember ExampleWithVectorMemberCollection::at(unsigned int index) {
  return ExampleWithVectorMember(m_entries.at(index));
}

int  ExampleWithVectorMemberCollection::size() const {
  return m_entries.size();
}

ExampleWithVectorMember ExampleWithVectorMemberCollection::create(){
  auto obj = new ExampleWithVectorMemberObj();
  m_entries.emplace_back(obj);
  m_vecs_count.push_back(obj->m_count);

  obj->id = {int(m_entries.size()-1),m_collectionID};
  return ExampleWithVectorMember(obj);
}

void ExampleWithVectorMemberCollection::clear(){
  m_data->clear();
  m_vec_count->clear();
  m_vecs_count.clear();

  for (auto& obj : m_entries) { delete obj; }
  m_entries.clear();
}

void ExampleWithVectorMemberCollection::prepareForWrite(){
  auto size = m_entries.size();
  m_data->reserve(size);
  for (auto& obj : m_entries) {m_data->push_back(obj->data); }
  for (auto& pointer : m_refCollections) {pointer->clear(); } 
  int count_size = std::accumulate( m_entries.begin(), m_entries.end(), 0, [](int sum, const ExampleWithVectorMemberObj*  obj){ return sum + obj->m_count->size();} );
  m_vec_count->reserve( count_size );
  int count_index =0;

  for(int i=0, size = m_data->size(); i != size; ++i){
   (*m_data)[i].count_begin=count_index;
   (*m_data)[i].count_end+=count_index;
   count_index = (*m_data)[i].count_end;
   for(auto it : (*m_vecs_count[i])) { m_vec_count->push_back(it); }

  }

}

void ExampleWithVectorMemberCollection::prepareAfterRead(){
  int index = 0;
  for (auto& data : *m_data){
    auto obj = new ExampleWithVectorMemberObj({index,m_collectionID}, data);
        obj->m_count = m_vec_count;

    m_entries.emplace_back(obj);
    ++index;
  }
  m_isValid = true;
}

bool ExampleWithVectorMemberCollection::setReferences(const podio::ICollectionProvider* collectionProvider){


  return true; //TODO: check success
}

void ExampleWithVectorMemberCollection::push_back(ConstExampleWithVectorMember object){
  int size = m_entries.size();
  auto obj = object.m_obj;
  if (obj->id.index == podio::ObjectID::untracked) {
      obj->id = {size,m_collectionID};
      m_entries.push_back(obj);
        m_vecs_count.push_back(obj->m_count);

  } else {
    throw std::invalid_argument( "Object already in a collection. Cannot add it to a second collection " );
  }
}

void ExampleWithVectorMemberCollection::setBuffer(void* address){
  if (m_data != nullptr) delete m_data;
  m_data = static_cast<ExampleWithVectorMemberDataContainer*>(address);
}


const ExampleWithVectorMember ExampleWithVectorMemberCollectionIterator::operator* () const {
  m_object.m_obj = (*m_collection)[m_index];
  return m_object;
}

const ExampleWithVectorMember* ExampleWithVectorMemberCollectionIterator::operator-> () const {
  m_object.m_obj = (*m_collection)[m_index];
  return &m_object;
}

const ExampleWithVectorMemberCollectionIterator& ExampleWithVectorMemberCollectionIterator::operator++() const {
  ++m_index;
  return *this;
}

std::ostream& operator<<( std::ostream& o,const ExampleWithVectorMemberCollection& v){
  std::ios::fmtflags old_flags = o.flags() ; 
  o << "id:          " << std::endl ;
   for(int i = 0; i < v.size(); i++){
     o << std::scientific << std::showpos  << std::setw(12)  << v[i].id() << " "   << std::endl;
  o << "     count : " ;
  for(unsigned j=0,N=v[i].count_size(); j<N ; ++j)
    o << v[i].count(j) << " " ; 
  o << std::endl ;
  }
o.flags(old_flags);
  return o ;
}



