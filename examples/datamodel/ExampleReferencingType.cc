// albers specific includes
#include "albers/Registry.h"

// datamodel specific includes
#include "ExampleReferencingTypeEntry.h"
#include "ExampleReferencingTypeData.h"
#include "ExampleReferencingTypeCollection.h"



std::vector<ExampleCluster>::const_iterator ExampleReferencingType::Clusters_begin() const {
  auto ret_value = m_entry->m_Clusters->begin();
  std::advance(ret_value, m_entry->data.Clusters_begin);
  return ret_value;
}

std::vector<ExampleCluster>::const_iterator ExampleReferencingType::Clusters_end() const {
  auto ret_value = m_entry->m_Clusters->begin();
  std::advance(ret_value, m_entry->data.Clusters_end-1);
  return ++ret_value;
}

void ExampleReferencingType::addClusters(ExampleCluster& component) {
  m_entry->m_Clusters->push_back(component);
  m_entry->data.Clusters_end++;
}

bool  ExampleReferencingType::isAvailable() const {
  if (m_entry != nullptr) {
    return true;
  }
  return false;
}

const albers::ObjectID ExampleReferencingType::getObjectID() const {
  return m_entry->id;
}


ExampleReferencingType::ExampleReferencingType(ExampleReferencingTypeEntry* entry) : m_entry(entry){
  if(m_entry != nullptr)
    m_entry->increaseRefCount();
}

ExampleReferencingType& ExampleReferencingType::operator=(const ExampleReferencingType& other){
  if ( m_entry != nullptr && m_entry->decreaseRefCount()==0) {
    std::cout << "deleting free-floating ExampleReferencingType at " << m_entry << std::endl;
    delete m_entry;
  }
  m_entry = other.m_entry;
  return *this;
}

ExampleReferencingType::~ExampleReferencingType(){
  if ( m_entry != nullptr && m_entry->decreaseRefCount()==0 ){
    std::cout << "deleting free-floating ExampleReferencingType at " << m_entry << std::endl;
    delete m_entry;
   }
}

//bool operator< (const ExampleReferencingType& p1, const ExampleReferencingType& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}
