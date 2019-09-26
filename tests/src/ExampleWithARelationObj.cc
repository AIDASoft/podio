#include "ExampleWithARelationObj.h"
#include "ExampleWithNamespaceConst.h"


namespace ex42 {
ExampleWithARelationObj::ExampleWithARelationObj() :
    ObjBase{{podio::ObjectID::untracked,podio::ObjectID::untracked},0}, data(), m_ref(nullptr)
, m_refs(new std::vector<::ex42::ConstExampleWithNamespace>())
{ }

ExampleWithARelationObj::ExampleWithARelationObj(const podio::ObjectID id, ExampleWithARelationData data) :
    ObjBase{id,0}, data(data)
{ }

ExampleWithARelationObj::ExampleWithARelationObj(const ExampleWithARelationObj& other) :
    ObjBase{{podio::ObjectID::untracked,podio::ObjectID::untracked},0}
    , data(other.data), m_ref(nullptr), m_refs(new std::vector<::ex42::ConstExampleWithNamespace>(*(other.m_refs)))
{
  if (other.m_ref != nullptr) {
     m_ref = new ::ex42::ConstExampleWithNamespace(*(other.m_ref));
  }

}

ExampleWithARelationObj::~ExampleWithARelationObj() {
  if (id.index == podio::ObjectID::untracked) {
    delete m_refs;

  }
    if (m_ref != nullptr) delete m_ref;

}
} // namespace ex42
