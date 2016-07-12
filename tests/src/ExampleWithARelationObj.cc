#include "ExampleWithARelationObj.h"
#include "ExampleWithNamespaceConst.h"


namespace ex {
ExampleWithARelationObj::ExampleWithARelationObj() :
    ObjBase{{podio::ObjectID::untracked,podio::ObjectID::untracked},0}, data(),m_ref(nullptr)
, m_refs(new std::vector<::ex::ConstExampleWithNamespace>())
{ }

ExampleWithARelationObj::ExampleWithARelationObj(const podio::ObjectID id, ExampleWithARelationData data) :
    ObjBase{id,0}, data(data)
{ }

ExampleWithARelationObj::ExampleWithARelationObj(const ExampleWithARelationObj& other) :
    ObjBase{{podio::ObjectID::untracked,podio::ObjectID::untracked},0}
    , data(other.data), m_refs(new std::vector<::ex::ConstExampleWithNamespace>(*(other.m_refs)))
{ }

ExampleWithARelationObj::~ExampleWithARelationObj() {
  if (id.index == podio::ObjectID::untracked) {
    delete m_refs;

  }
    if (m_ref != nullptr) delete m_ref;

}
} // namespace ex
