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

ExampleWithArray::ExampleWithArray(NotSoSimpleStruct arrayStruct,std::array<int, 4> myArray,std::array<int, 4> anotherArray2,std::array<int, 4> snail_case_array,std::array<int, 4> snail_case_Array3) : m_obj(new ExampleWithArrayObj()) {
  m_obj->acquire();
    m_obj->data.arrayStruct = arrayStruct;  m_obj->data.myArray = myArray;  m_obj->data.anotherArray2 = anotherArray2;  m_obj->data.snail_case_array = snail_case_array;  m_obj->data.snail_case_Array3 = snail_case_Array3;
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


//bool operator< (const ExampleWithArray& p1, const ExampleWithArray& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}


