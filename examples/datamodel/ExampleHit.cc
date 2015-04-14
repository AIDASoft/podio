// datamodel specific includes
#include "ExampleHitObj.h"
#include "ExampleHitData.h"
#include "ExampleHitCollection.h"
#include <iostream>

ExampleHit::ExampleHit() : m_obj(new ExampleHitObj()){};

ExampleHit::ExampleHit(const ExampleHit& other) : m_obj(other.m_obj) {
  m_obj->increaseRefCount();
};

const double& ExampleHit::x() const { return m_obj->data.x;}
const double& ExampleHit::y() const { return m_obj->data.y;}
const double& ExampleHit::z() const { return m_obj->data.z;}
const double& ExampleHit::energy() const { return m_obj->data.energy;}

void ExampleHit::x(double value){ m_obj->data.x = value;}
void ExampleHit::y(double value){ m_obj->data.y = value;}
void ExampleHit::z(double value){ m_obj->data.z = value;}
void ExampleHit::energy(double value){ m_obj->data.energy = value;}


bool  ExampleHit::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const albers::ObjectID ExampleHit::getObjectID() const {
  return m_obj->id;
}


ExampleHit::ExampleHit(ExampleHitObj* obj) : m_obj(obj){
  if(m_obj != nullptr)
    m_obj->increaseRefCount();
}

ExampleHit& ExampleHit::operator=(const ExampleHit& other){
  if ( m_obj != nullptr && m_obj->decreaseRefCount()==0) {
    std::cout << "deleting free-floating ExampleHit at " << m_obj << std::endl;
    delete m_obj;
  }
  m_obj = other.m_obj;
  return *this;
}

ExampleHit::~ExampleHit(){
  if ( m_obj != nullptr && m_obj->decreaseRefCount()==0 ){
    std::cout << "deleting free-floating ExampleHit at " << m_obj << std::endl;
    delete m_obj;
   }
}

//bool operator< (const ExampleHit& p1, const ExampleHit& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}
