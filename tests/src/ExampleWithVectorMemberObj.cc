#include "ExampleWithVectorMemberObj.h"

ExampleWithVectorMemberObj::ExampleWithVectorMemberObj()
    : ObjBase{{podio::ObjectID::untracked, podio::ObjectID::untracked}, 0},
      data(), m_count(new std::vector<int>()) {}

ExampleWithVectorMemberObj::ExampleWithVectorMemberObj(
    const podio::ObjectID id, ExampleWithVectorMemberData data)
    : ObjBase{id, 0}, data(data) {}

ExampleWithVectorMemberObj::ExampleWithVectorMemberObj(
    const ExampleWithVectorMemberObj &other)
    : ObjBase{{podio::ObjectID::untracked, podio::ObjectID::untracked}, 0},
      data(other.data), m_count(new std::vector<int>(*(other.m_count))) {}

ExampleWithVectorMemberObj::~ExampleWithVectorMemberObj() {
  if (id.index == podio::ObjectID::untracked) {
    delete m_count;
  }
}
