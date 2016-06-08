#include "ExampleWithARelationObj.h"
#include "ExampleWithNamespace.h"


namespace ex {
ExampleWithARelationObj::ExampleWithARelationObj() :
    ObjBase{{podio::ObjectID::untracked,podio::ObjectID::untracked},0}, data(),m_ref(nullptr)

{ }

ExampleWithARelationObj::ExampleWithARelationObj(const podio::ObjectID id, ExampleWithARelationData data) :
    ObjBase{id,0}, data(data)
{ }

ExampleWithARelationObj::ExampleWithARelationObj(const ExampleWithARelationObj& other) :
    ObjBase{{podio::ObjectID::untracked,podio::ObjectID::untracked},0}
    , data(other.data)
{ }

ExampleWithARelationObj::~ExampleWithARelationObj() {
  if (id.index == podio::ObjectID::untracked) {

  }
    if (m_ref != nullptr) delete m_ref;

}
} // namespace ex
