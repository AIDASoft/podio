// albers specific includes
#include "albers/Registry.h"

// datamodel specific includes
#include "ExampleHitEntry.h"
#include "ExampleHitData.h"
#include "ExampleHitCollection.h"

const double& ExampleHit::x() const { return m_entry->data.x;}
const double& ExampleHit::y() const { return m_entry->data.y;}
const double& ExampleHit::z() const { return m_entry->data.z;}
const double& ExampleHit::energy() const { return m_entry->data.energy;}

void ExampleHit::x(double value){ m_entry->data.x = value;}
void ExampleHit::y(double value){ m_entry->data.y = value;}
void ExampleHit::z(double value){ m_entry->data.z = value;}
void ExampleHit::energy(double value){ m_entry->data.energy = value;}


bool  ExampleHit::isAvailable() const {
  if (m_entry != nullptr) {
    return true;
  }
  return false;
}

const albers::ObjectID ExampleHit::getObjectID() const {
  return m_entry->id;
}


ExampleHit::ExampleHit(ExampleHitEntry* entry) : m_entry(entry){
  if(m_entry != nullptr)
    m_entry->increaseRefCount();
}

ExampleHit& ExampleHit::operator=(const ExampleHit& other){
  if ( m_entry != nullptr && m_entry->decreaseRefCount()==0) {
    std::cout << "deleting free-floating ExampleHit at " << m_entry << std::endl;
    delete m_entry;
  }
  m_entry = other.m_entry;
  return *this;
}

ExampleHit::~ExampleHit(){
  if ( m_entry != nullptr && m_entry->decreaseRefCount()==0 ){
    std::cout << "deleting free-floating ExampleHit at " << m_entry << std::endl;
    delete m_entry;
   }
}

//bool operator< (const ExampleHit& p1, const ExampleHit& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}
