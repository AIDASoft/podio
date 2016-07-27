#include "ExampleMCObj.h"
#include "ExampleMC.h"
#include "ExampleMC.h"



ExampleMCObj::ExampleMCObj() :
    ObjBase{{podio::ObjectID::untracked,podio::ObjectID::untracked},0}, data(), m_parents(new std::vector<ConstExampleMC>()), m_daughters(new std::vector<ConstExampleMC>())
{ }

ExampleMCObj::ExampleMCObj(const podio::ObjectID id, ExampleMCData data) :
    ObjBase{id,0}, data(data)
{ }

ExampleMCObj::ExampleMCObj(const ExampleMCObj& other) :
    ObjBase{{podio::ObjectID::untracked,podio::ObjectID::untracked},0}
    , data(other.data), m_parents(new std::vector<ConstExampleMC>(*(other.m_parents))), m_daughters(new std::vector<ConstExampleMC>(*(other.m_daughters)))
{

}

ExampleMCObj::~ExampleMCObj() {
  if (id.index == podio::ObjectID::untracked) {
    delete m_parents;
    delete m_daughters;

  }

}

