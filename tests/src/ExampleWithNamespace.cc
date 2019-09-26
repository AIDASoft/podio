// datamodel specific includes
#include "ExampleWithNamespace.h"
#include "ExampleWithNamespaceConst.h"
#include "ExampleWithNamespaceObj.h"
#include "ExampleWithNamespaceData.h"
#include "ExampleWithNamespaceCollection.h"
#include <iostream>


namespace ex42 {

ExampleWithNamespace::ExampleWithNamespace() : m_obj(new ExampleWithNamespaceObj()){
 m_obj->acquire();
}

ExampleWithNamespace::ExampleWithNamespace(ex2::NamespaceStruct data) : m_obj(new ExampleWithNamespaceObj()) {
  m_obj->acquire();
    m_obj->data.data = data;
}


ExampleWithNamespace::ExampleWithNamespace(const ExampleWithNamespace& other) : m_obj(other.m_obj) {
  m_obj->acquire();
}

ExampleWithNamespace& ExampleWithNamespace::operator=(const ExampleWithNamespace& other) {
  if ( m_obj != nullptr) m_obj->release();
  m_obj = other.m_obj;
  return *this;
}

ExampleWithNamespace::ExampleWithNamespace(ExampleWithNamespaceObj* obj) : m_obj(obj){
  if(m_obj != nullptr)
    m_obj->acquire();
}

ExampleWithNamespace ExampleWithNamespace::clone() const {
  return {new ExampleWithNamespaceObj(*m_obj)};
}

ExampleWithNamespace::~ExampleWithNamespace(){
  if ( m_obj != nullptr) m_obj->release();
}

ExampleWithNamespace::operator ConstExampleWithNamespace() const {return ConstExampleWithNamespace(m_obj);}

  const ex2::NamespaceStruct& ExampleWithNamespace::data() const { return m_obj->data.data; }
const int& ExampleWithNamespace::x() const { return m_obj->data.data.x; }
const int& ExampleWithNamespace::y() const { return m_obj->data.data.y; }

  ex2::NamespaceStruct& ExampleWithNamespace::data() { return m_obj->data.data; }
void ExampleWithNamespace::data(class ex2::NamespaceStruct value) { m_obj->data.data = value; }
void ExampleWithNamespace::x(int value){ m_obj->data.data.x = value; }
void ExampleWithNamespace::y(int value){ m_obj->data.data.y = value; }



bool  ExampleWithNamespace::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const podio::ObjectID ExampleWithNamespace::getObjectID() const {
  if (m_obj !=nullptr){
    return m_obj->id;
  }
  return podio::ObjectID{-2,-2};
}

bool ExampleWithNamespace::operator==(const ConstExampleWithNamespace& other) const {
  return (m_obj==other.m_obj);
}

std::ostream& operator<<( std::ostream& o,const ConstExampleWithNamespace& value ){
  o << " id : " << value.id() << std::endl ;
  o << " data : " << value.data() << std::endl ;
  o << " x : " << value.x() << std::endl ;
  o << " y : " << value.y() << std::endl ;
  return o ;
}


//bool operator< (const ExampleWithNamespace& p1, const ExampleWithNamespace& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}

} // namespace ex42
