// albers specific includes
#include "albers/Registry.h"

// datamodel specific includes
#include "ExampleClusterEntry.h"
#include "ExampleClusterData.h"
#include "ExampleClusterCollection.h"

const double& ExampleCluster::energy() const { return m_entry->data.energy;}

void ExampleCluster::energy(double value){ m_entry->data.energy = value;}

std::vector<ExampleHit>::const_iterator ExampleCluster::Hits_begin() const {
  auto ret_value = m_entry->m_Hits->begin();
  std::advance(ret_value, m_entry->data.Hits_begin);
  return ret_value;
}

std::vector<ExampleHit>::const_iterator ExampleCluster::Hits_end() const {
  auto ret_value = m_entry->m_Hits->begin();
  std::advance(ret_value, m_entry->data.Hits_end-1);
  return ++ret_value;
}

void ExampleCluster::addHits(ExampleHit& component) {
  m_entry->m_Hits->push_back(component);
  m_entry->data.Hits_end++;
}

bool  ExampleCluster::isAvailable() const {
  if (m_entry != nullptr) {
    return true;
  }
  return false;
}

const albers::ObjectID ExampleCluster::getObjectID() const {
  return m_entry->id;
}


ExampleCluster::ExampleCluster(ExampleClusterEntry* entry) : m_entry(entry){
  if(m_entry != nullptr)
    m_entry->increaseRefCount();
}

ExampleCluster& ExampleCluster::operator=(const ExampleCluster& other){
  if ( m_entry != nullptr && m_entry->decreaseRefCount()==0) {
    std::cout << "deleting free-floating ExampleCluster at " << m_entry << std::endl;
    delete m_entry;
  }
  m_entry = other.m_entry;
  return *this;
}

ExampleCluster::~ExampleCluster(){
  if ( m_entry != nullptr && m_entry->decreaseRefCount()==0 ){
    std::cout << "deleting free-floating ExampleCluster at " << m_entry << std::endl;
    delete m_entry;
   }
}

//bool operator< (const ExampleCluster& p1, const ExampleCluster& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}
