// datamodel specific includes
#include "ExampleMC.h"
#include "ExampleMCConst.h"
#include "ExampleMCObj.h"
#include "ExampleMCData.h"
#include "ExampleMCCollection.h"
#include <iostream>




ConstExampleMC::ConstExampleMC() : m_obj(new ExampleMCObj()) {
 m_obj->acquire();
}

ConstExampleMC::ConstExampleMC(double energy,int PDG) : m_obj(new ExampleMCObj()){
 m_obj->acquire();
   m_obj->data.energy = energy;  m_obj->data.PDG = PDG;
}


ConstExampleMC::ConstExampleMC(const ConstExampleMC& other) : m_obj(other.m_obj) {
  m_obj->acquire();
}

ConstExampleMC& ConstExampleMC::operator=(const ConstExampleMC& other) {
  if ( m_obj != nullptr) m_obj->release();
  m_obj = other.m_obj;
  return *this;
}

ConstExampleMC::ConstExampleMC(ExampleMCObj* obj) : m_obj(obj) {
  if(m_obj != nullptr)
    m_obj->acquire();
}

ConstExampleMC ConstExampleMC::clone() const {
  return {new ExampleMCObj(*m_obj)};
}

ConstExampleMC::~ConstExampleMC(){
  if ( m_obj != nullptr) m_obj->release();
}

  /// Access the  energy
  const double& ConstExampleMC::energy() const { return m_obj->data.energy; }
  /// Access the  PDG code
  const int& ConstExampleMC::PDG() const { return m_obj->data.PDG; }

std::vector<::ConstExampleMC>::const_iterator ConstExampleMC::parents_begin() const {
  auto ret_value = m_obj->m_parents->begin();
  std::advance(ret_value, m_obj->data.parents_begin);
  return ret_value;
}

std::vector<::ConstExampleMC>::const_iterator ConstExampleMC::parents_end() const {
  auto ret_value = m_obj->m_parents->begin();
  std::advance(ret_value, m_obj->data.parents_end-1);
  return ++ret_value;
}

unsigned int ConstExampleMC::parents_size() const {
  return (m_obj->data.parents_end-m_obj->data.parents_begin);
}

::ConstExampleMC ConstExampleMC::parents(unsigned int index) const {
  if (parents_size() > index) {
    return m_obj->m_parents->at(m_obj->data.parents_begin+index);
  }
  else throw std::out_of_range ("index out of bounds for existing references");
}
std::vector<::ConstExampleMC>::const_iterator ConstExampleMC::daughters_begin() const {
  auto ret_value = m_obj->m_daughters->begin();
  std::advance(ret_value, m_obj->data.daughters_begin);
  return ret_value;
}

std::vector<::ConstExampleMC>::const_iterator ConstExampleMC::daughters_end() const {
  auto ret_value = m_obj->m_daughters->begin();
  std::advance(ret_value, m_obj->data.daughters_end-1);
  return ++ret_value;
}

unsigned int ConstExampleMC::daughters_size() const {
  return (m_obj->data.daughters_end-m_obj->data.daughters_begin);
}

::ConstExampleMC ConstExampleMC::daughters(unsigned int index) const {
  if (daughters_size() > index) {
    return m_obj->m_daughters->at(m_obj->data.daughters_begin+index);
  }
  else throw std::out_of_range ("index out of bounds for existing references");
}


bool  ConstExampleMC::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const podio::ObjectID ConstExampleMC::getObjectID() const {
  if (m_obj !=nullptr){
    return m_obj->id;
  }
  return podio::ObjectID{-2,-2};
}

bool ConstExampleMC::operator==(const ExampleMC& other) const {
     return (m_obj==other.m_obj);
}

//bool operator< (const ExampleMC& p1, const ExampleMC& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}


