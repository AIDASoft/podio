// datamodel specific includes
#include "ExampleWithVectorMember.h"
#include "ExampleWithVectorMemberCollection.h"
#include "ExampleWithVectorMemberConst.h"
#include "ExampleWithVectorMemberData.h"
#include "ExampleWithVectorMemberObj.h"
#include <iostream>

ExampleWithVectorMember::ExampleWithVectorMember()
    : m_obj(new ExampleWithVectorMemberObj()) {
  m_obj->acquire();
}

ExampleWithVectorMember::ExampleWithVectorMember(
    const ExampleWithVectorMember &other)
    : m_obj(other.m_obj) {
  m_obj->acquire();
}

ExampleWithVectorMember &ExampleWithVectorMember::
operator=(const ExampleWithVectorMember &other) {
  if (m_obj != nullptr)
    m_obj->release();
  m_obj = other.m_obj;
  return *this;
}

ExampleWithVectorMember::ExampleWithVectorMember(
    ExampleWithVectorMemberObj *obj)
    : m_obj(obj) {
  if (m_obj != nullptr)
    m_obj->acquire();
}

ExampleWithVectorMember ExampleWithVectorMember::clone() const {
  return {new ExampleWithVectorMemberObj(*m_obj)};
}

ExampleWithVectorMember::~ExampleWithVectorMember() {
  if (m_obj != nullptr)
    m_obj->release();
}

ExampleWithVectorMember::operator ConstExampleWithVectorMember() const {
  return ConstExampleWithVectorMember(m_obj);
}

std::vector<int>::const_iterator ExampleWithVectorMember::count_begin() const {
  auto ret_value = m_obj->m_count->begin();
  std::advance(ret_value, m_obj->data.count_begin);
  return ret_value;
}

std::vector<int>::const_iterator ExampleWithVectorMember::count_end() const {
  auto ret_value = m_obj->m_count->begin();
  std::advance(ret_value, m_obj->data.count_end);
  return ret_value;
}

void ExampleWithVectorMember::addcount(int component) {
  m_obj->m_count->push_back(component);
  m_obj->data.count_end++;
}

unsigned int ExampleWithVectorMember::count_size() const {
  return (m_obj->data.count_end - m_obj->data.count_begin);
}

int ExampleWithVectorMember::count(unsigned int index) const {
  if (count_size() > index) {
    return m_obj->m_count->at(m_obj->data.count_begin + index);
  } else
    throw std::out_of_range("index out of bounds for existing references");
}

bool ExampleWithVectorMember::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const podio::ObjectID ExampleWithVectorMember::getObjectID() const {
  if (m_obj != nullptr) {
    return m_obj->id;
  }
  return podio::ObjectID{-2, -2};
}

bool ExampleWithVectorMember::
operator==(const ConstExampleWithVectorMember &other) const {
  return (m_obj == other.m_obj);
}

std::ostream &operator<<(std::ostream &o,
                         const ConstExampleWithVectorMember &value) {
  o << " id : " << value.id() << std::endl;
  o << " count : ";
  for (unsigned i = 0, N = value.count_size(); i < N; ++i)
    o << value.count(i) << " ";
  o << std::endl;
  return o;
}

// bool operator< (const ExampleWithVectorMember& p1, const
// ExampleWithVectorMember& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}
