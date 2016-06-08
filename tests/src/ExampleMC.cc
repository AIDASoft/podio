// datamodel specific includes
#include "ExampleMC.h"
#include "ExampleMCObj.h"
#include "ExampleMCData.h"
#include "ExampleMCCollection.h"
#include <iostream>




ExampleMC::ExampleMC() : m_obj(new ExampleMCObj()){
 m_obj->acquire();
}

ExampleMC::ExampleMC(double energy,int PDG) : m_obj(new ExampleMCObj()) {
  m_obj->acquire();
    m_obj->data.energy = energy;  m_obj->data.PDG = PDG;
}


ExampleMC::ExampleMC(const ExampleMC& other) : m_obj(other.m_obj) {
  m_obj->acquire();
}

ExampleMC& ExampleMC::operator=(const ExampleMC& other) {
  if ( m_obj != nullptr) m_obj->release();
  m_obj = other.m_obj;
  return *this;
}

ExampleMC::ExampleMC(ExampleMCObj* obj) : m_obj(obj){
  if(m_obj != nullptr)
    m_obj->acquire();
}

ExampleMC ExampleMC::clone() const {
  return {new ExampleMCObj(*m_obj)};
}

ExampleMC::~ExampleMC(){
  if ( m_obj != nullptr) m_obj->release();
}

//ExampleMC::operator ExampleMC() const {return ExampleMC(m_obj);}

  const double& ExampleMC::energy() const { return m_obj->data.energy; }
  const int& ExampleMC::PDG() const { return m_obj->data.PDG; }

void ExampleMC::energy(double value){ m_obj->data.energy = value; }
void ExampleMC::PDG(int value){ m_obj->data.PDG = value; }

std::vector<ExampleMC>::const_iterator ExampleMC::parents_begin() const {
  auto ret_value = m_obj->m_parents->begin();
  std::advance(ret_value, m_obj->data.parents_begin);
  return ret_value;
}

std::vector<ExampleMC>::const_iterator ExampleMC::parents_end() const {
  auto ret_value = m_obj->m_parents->begin();
//fg: this code fails if m_obj->data.parents==0
//  std::advance(ret_value, m_obj->data.parents_end-1);
//  return ++ret_value;
  std::advance(ret_value, m_obj->data.parents_end);
  return ret_value;
}

void ExampleMC::addparents(ExampleMC component) {
  m_obj->m_parents->push_back(component);
  m_obj->data.parents_end++;
}

unsigned int ExampleMC::parents_size() const {
  return (m_obj->data.parents_end-m_obj->data.parents_begin);
}

ExampleMC ExampleMC::parents(unsigned int index) const {
  if (parents_size() > index) {
    return m_obj->m_parents->at(m_obj->data.parents_begin+index);
  }
  else throw std::out_of_range ("index out of bounds for existing references");
}
std::vector<ExampleMC>::const_iterator ExampleMC::daughters_begin() const {
  auto ret_value = m_obj->m_daughters->begin();
  std::advance(ret_value, m_obj->data.daughters_begin);
  return ret_value;
}

std::vector<ExampleMC>::const_iterator ExampleMC::daughters_end() const {
  auto ret_value = m_obj->m_daughters->begin();
//fg: this code fails if m_obj->data.daughters==0
//  std::advance(ret_value, m_obj->data.daughters_end-1);
//  return ++ret_value;
  std::advance(ret_value, m_obj->data.daughters_end);
  return ret_value;
}

void ExampleMC::adddaughters(ExampleMC component) {
  m_obj->m_daughters->push_back(component);
  m_obj->data.daughters_end++;
}

unsigned int ExampleMC::daughters_size() const {
  return (m_obj->data.daughters_end-m_obj->data.daughters_begin);
}

ExampleMC ExampleMC::daughters(unsigned int index) const {
  if (daughters_size() > index) {
    return m_obj->m_daughters->at(m_obj->data.daughters_begin+index);
  }
  else throw std::out_of_range ("index out of bounds for existing references");
}


bool  ExampleMC::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const podio::ObjectID ExampleMC::getObjectID() const {
  if (m_obj !=nullptr){
    return m_obj->id;
  }
  return podio::ObjectID{-2,-2};
}


//bool operator< (const ExampleMC& p1, const ExampleMC& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}


