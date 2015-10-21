// datamodel specific includes
#include "ExampleHit.h"
#include "ExampleHitConst.h"
#include "ExampleHitObj.h"
#include "ExampleHitData.h"
#include "ExampleHitCollection.h"
#include <iostream>

ExampleHit::ExampleHit() : m_obj(new ExampleHitObj()){
 m_obj->acquire();
};

ExampleHit::ExampleHit(double x,double y,double z,double energy) : m_obj(new ExampleHitObj()){
 m_obj->acquire();
   m_obj->data.x = x;  m_obj->data.y = y;  m_obj->data.z = z;  m_obj->data.energy = energy;
};

ExampleHit::ExampleHit(const ExampleHit& other) : m_obj(other.m_obj) {
  m_obj->acquire();
}

ExampleHit& ExampleHit::operator=(const ExampleHit& other) {
  if ( m_obj != nullptr) m_obj->release();
  m_obj = other.m_obj;
  return *this;
}

ExampleHit::ExampleHit(ExampleHitObj* obj) : m_obj(obj){
  if(m_obj != nullptr)
    m_obj->acquire();
}

ExampleHit ExampleHit::clone() const {
  return {new ExampleHitObj(*m_obj)};
}

ExampleHit::~ExampleHit(){
  if ( m_obj != nullptr) m_obj->release();
}

ExampleHit::operator ConstExampleHit() const {return ConstExampleHit(m_obj);};




bool  ExampleHit::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const podio::ObjectID ExampleHit::getObjectID() const {
  if (m_obj !=nullptr){
    return m_obj->id;
  }
  return podio::ObjectID{-2,-2};
}

bool ExampleHit::operator==(const ConstExampleHit& other) const {
     return (m_obj==other.m_obj);
}


//bool operator< (const ExampleHit& p1, const ExampleHit& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}
