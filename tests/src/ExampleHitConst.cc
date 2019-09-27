// datamodel specific includes
#include "ExampleHit.h"
#include "ExampleHitConst.h"
#include "ExampleHitObj.h"
#include "ExampleHitData.h"
#include "ExampleHitCollection.h"
#include <iostream>




ConstExampleHit::ConstExampleHit() : m_obj(new ExampleHitObj()) {
 m_obj->acquire();
}

ConstExampleHit::ConstExampleHit(unsigned long long cellID,double x,double y,double z,double energy) : m_obj(new ExampleHitObj()){
 m_obj->acquire();
   m_obj->data.cellID = cellID;  m_obj->data.x = x;  m_obj->data.y = y;  m_obj->data.z = z;  m_obj->data.energy = energy;
}


ConstExampleHit::ConstExampleHit(const ConstExampleHit& other) : m_obj(other.m_obj) {
  m_obj->acquire();
}

ConstExampleHit& ConstExampleHit::operator=(const ConstExampleHit& other) {
  if ( m_obj != nullptr) m_obj->release();
  m_obj = other.m_obj;
  return *this;
}

ConstExampleHit::ConstExampleHit(ExampleHitObj* obj) : m_obj(obj) {
  if(m_obj != nullptr)
    m_obj->acquire();
}

ConstExampleHit ConstExampleHit::clone() const {
  return {new ExampleHitObj(*m_obj)};
}

ConstExampleHit::~ConstExampleHit(){
  if ( m_obj != nullptr) m_obj->release();
}

  /// Access the  cellID
  const unsigned long long& ConstExampleHit::cellID() const { return m_obj->data.cellID; }
  /// Access the  x-coordinate
  const double& ConstExampleHit::x() const { return m_obj->data.x; }
  /// Access the  y-coordinate
  const double& ConstExampleHit::y() const { return m_obj->data.y; }
  /// Access the  z-coordinate
  const double& ConstExampleHit::z() const { return m_obj->data.z; }
  /// Access the  measured energy deposit
  const double& ConstExampleHit::energy() const { return m_obj->data.energy; }



bool  ConstExampleHit::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const podio::ObjectID ConstExampleHit::getObjectID() const {
  if (m_obj !=nullptr){
    return m_obj->id;
  }
  return podio::ObjectID{-2,-2};
}

bool ConstExampleHit::operator==(const ExampleHit& other) const {
     return (m_obj==other.m_obj);
}

//bool operator< (const ExampleHit& p1, const ExampleHit& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}


