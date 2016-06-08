#ifndef ExampleWithARelationOBJ_H
#define ExampleWithARelationOBJ_H

// std includes
#include <atomic>
#include <iostream>

// data model specific includes
#include "podio/ObjBase.h"
#include "ExampleWithARelationData.h"



// forward declarations
class ExampleWithARelation;
namespace ex {class ExampleWithNamespace;
}


namespace ex {
class ExampleWithARelationObj : public podio::ObjBase {
public:
  /// constructor
  ExampleWithARelationObj();
  /// copy constructor (does a deep-copy of relation containers)
  ExampleWithARelationObj(const ExampleWithARelationObj&);
  /// constructor from ObjectID and ExampleWithARelationData
  /// does not initialize the internal relation containers
  ExampleWithARelationObj(const podio::ObjectID id, ExampleWithARelationData data);
  virtual ~ExampleWithARelationObj();

public:
  ExampleWithARelationData data;
  bool isMutable;
  ::ex::ExampleWithNamespace* m_ref;


};
} // namespace ex


#endif
