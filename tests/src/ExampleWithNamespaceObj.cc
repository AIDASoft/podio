#include "ExampleWithNamespaceObj.h"


namespace ex42 {
ExampleWithNamespaceObj::ExampleWithNamespaceObj() :
    ObjBase{{podio::ObjectID::untracked,podio::ObjectID::untracked},0}, data()
{ }

ExampleWithNamespaceObj::ExampleWithNamespaceObj(const podio::ObjectID id, ExampleWithNamespaceData data) :
    ObjBase{id,0}, data(data)
{ }

ExampleWithNamespaceObj::ExampleWithNamespaceObj(const ExampleWithNamespaceObj& other) :
    ObjBase{{podio::ObjectID::untracked,podio::ObjectID::untracked},0}
    , data(other.data)
{

}

ExampleWithNamespaceObj::~ExampleWithNamespaceObj() {
  if (id.index == podio::ObjectID::untracked) {

  }

}
} // namespace ex42
