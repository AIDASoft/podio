// datamodel specific includes
#include "ExampleWithArray.h"
#include "ExampleWithArrayConst.h"
#include "ExampleWithArrayObj.h"
#include "ExampleWithArrayData.h"
#include "ExampleWithArrayCollection.h"
#include <iostream>




ExampleWithArray::ExampleWithArray() : m_obj(new ExampleWithArrayObj()){
 m_obj->acquire();
}

ExampleWithArray::ExampleWithArray(NotSoSimpleStruct arrayStruct,std::array<int, 4> myArray,std::array<int, 4> anotherArray2,std::array<int, 4> snail_case_array,std::array<int, 4> snail_case_Array3,std::array<ex2::NamespaceStruct, 4> structArray) : m_obj(new ExampleWithArrayObj()) {
  m_obj->acquire();
    m_obj->data.arrayStruct = arrayStruct;  m_obj->data.myArray = myArray;  m_obj->data.anotherArray2 = anotherArray2;  m_obj->data.snail_case_array = snail_case_array;  m_obj->data.snail_case_Array3 = snail_case_Array3;  m_obj->data.structArray = structArray;
}


ExampleWithArray::ExampleWithArray(const ExampleWithArray& other) : m_obj(other.m_obj) {
  m_obj->acquire();
}

ExampleWithArray& ExampleWithArray::operator=(const ExampleWithArray& other) {
  if ( m_obj != nullptr) m_obj->release();
  m_obj = other.m_obj;
  return *this;
}

ExampleWithArray::ExampleWithArray(ExampleWithArrayObj* obj) : m_obj(obj){
  if(m_obj != nullptr)
    m_obj->acquire();
}

ExampleWithArray ExampleWithArray::clone() const {
  return {new ExampleWithArrayObj(*m_obj)};
}

ExampleWithArray::~ExampleWithArray(){
  if ( m_obj != nullptr) m_obj->release();
}

ExampleWithArray::operator ConstExampleWithArray() const {return ConstExampleWithArray(m_obj);}

  const NotSoSimpleStruct& ExampleWithArray::arrayStruct() const { return m_obj->data.arrayStruct; }
const SimpleStruct& ExampleWithArray::data() const { return m_obj->data.arrayStruct.data; }
  const std::array<int, 4>& ExampleWithArray::myArray() const { return m_obj->data.myArray; }
  const int& ExampleWithArray::myArray(size_t i) const { return m_obj->data.myArray.at(i); }
  const std::array<int, 4>& ExampleWithArray::anotherArray2() const { return m_obj->data.anotherArray2; }
  const int& ExampleWithArray::anotherArray2(size_t i) const { return m_obj->data.anotherArray2.at(i); }
  const std::array<int, 4>& ExampleWithArray::snail_case_array() const { return m_obj->data.snail_case_array; }
  const int& ExampleWithArray::snail_case_array(size_t i) const { return m_obj->data.snail_case_array.at(i); }
  const std::array<int, 4>& ExampleWithArray::snail_case_Array3() const { return m_obj->data.snail_case_Array3; }
  const int& ExampleWithArray::snail_case_Array3(size_t i) const { return m_obj->data.snail_case_Array3.at(i); }
  const std::array<ex2::NamespaceStruct, 4>& ExampleWithArray::structArray() const { return m_obj->data.structArray; }
  const ex2::NamespaceStruct& ExampleWithArray::structArray(size_t i) const { return m_obj->data.structArray.at(i); }

  NotSoSimpleStruct& ExampleWithArray::arrayStruct() { return m_obj->data.arrayStruct; }
void ExampleWithArray::arrayStruct(class NotSoSimpleStruct value) { m_obj->data.arrayStruct = value; }
SimpleStruct& ExampleWithArray::data() { return m_obj->data.arrayStruct.data; }
void ExampleWithArray::data(class SimpleStruct value) { m_obj->data.arrayStruct.data = value; }
void ExampleWithArray::myArray(std::array<int, 4> value) { m_obj->data.myArray = value; }
void ExampleWithArray::myArray(size_t i, int value) { m_obj->data.myArray.at(i) = value; }
void ExampleWithArray::anotherArray2(std::array<int, 4> value) { m_obj->data.anotherArray2 = value; }
void ExampleWithArray::anotherArray2(size_t i, int value) { m_obj->data.anotherArray2.at(i) = value; }
void ExampleWithArray::snail_case_array(std::array<int, 4> value) { m_obj->data.snail_case_array = value; }
void ExampleWithArray::snail_case_array(size_t i, int value) { m_obj->data.snail_case_array.at(i) = value; }
void ExampleWithArray::snail_case_Array3(std::array<int, 4> value) { m_obj->data.snail_case_Array3 = value; }
void ExampleWithArray::snail_case_Array3(size_t i, int value) { m_obj->data.snail_case_Array3.at(i) = value; }
void ExampleWithArray::structArray(std::array<ex2::NamespaceStruct, 4> value) { m_obj->data.structArray = value; }
void ExampleWithArray::structArray(size_t i, ex2::NamespaceStruct value) { m_obj->data.structArray.at(i) = value; }



bool  ExampleWithArray::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const podio::ObjectID ExampleWithArray::getObjectID() const {
  if (m_obj !=nullptr){
    return m_obj->id;
  }
  return podio::ObjectID{-2,-2};
}

bool ExampleWithArray::operator==(const ConstExampleWithArray& other) const {
  return (m_obj==other.m_obj);
}

std::ostream& operator<<( std::ostream& o,const ConstExampleWithArray& value ){
  o << " id : " << value.id() << std::endl ;
  o << " arrayStruct : " << value.arrayStruct() << std::endl ;
  o << " data : " << value.data() << std::endl ;
  o << " myArray : " ;
  for(int i=0,N= 4;i<N;++i)
      o << value.myArray()[i] << "|" ;
  o << std::endl ;
  o << " anotherArray2 : " ;
  for(int i=0,N= 4;i<N;++i)
      o << value.anotherArray2()[i] << "|" ;
  o << std::endl ;
  o << " snail_case_array : " ;
  for(int i=0,N= 4;i<N;++i)
      o << value.snail_case_array()[i] << "|" ;
  o << std::endl ;
  o << " snail_case_Array3 : " ;
  for(int i=0,N= 4;i<N;++i)
      o << value.snail_case_Array3()[i] << "|" ;
  o << std::endl ;
  o << " structArray : " ;
  for(int i=0,N= 4;i<N;++i)
      o << value.structArray()[i] << "|" ;
  o << std::endl ;
  return o ;
}


//bool operator< (const ExampleWithArray& p1, const ExampleWithArray& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}


