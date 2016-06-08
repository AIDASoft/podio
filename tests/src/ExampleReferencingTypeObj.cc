#include "ExampleReferencingTypeObj.h"
#include "ExampleReferencingType.h"



ExampleReferencingTypeObj::ExampleReferencingTypeObj() :
    ObjBase{{podio::ObjectID::untracked,podio::ObjectID::untracked},0}, data(), m_Clusters(new std::vector<ExampleCluster>()), m_Refs(new std::vector<ExampleReferencingType>())
{ }

ExampleReferencingTypeObj::ExampleReferencingTypeObj(const podio::ObjectID id, ExampleReferencingTypeData data) :
    ObjBase{id,0}, data(data)
{ }

ExampleReferencingTypeObj::ExampleReferencingTypeObj(const ExampleReferencingTypeObj& other) :
    ObjBase{{podio::ObjectID::untracked,podio::ObjectID::untracked},0}
    , data(other.data), m_Clusters(new std::vector<ExampleCluster>(*(other.m_Clusters))), m_Refs(new std::vector<ExampleReferencingType>(*(other.m_Refs)))
{ }

ExampleReferencingTypeObj::~ExampleReferencingTypeObj() {
  if (id.index == podio::ObjectID::untracked) {
    delete m_Clusters;
    delete m_Refs;

  }

}

