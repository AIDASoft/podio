#ifndef ExampleWithARelationOBJ_H
#define ExampleWithARelationOBJ_H

// std includes
#include <atomic>
#include <iostream>

// data model specific includes
#include "podio/ObjBase.h"
#include "ExampleWithARelationData.h"

#include <vector>
#include "ExampleWithNamespace.h"


// forward declarations
namespace ex42 {class ConstExampleWithNamespace;
}


namespace ex42 {

class ExampleWithARelation;
class ConstExampleWithARelation;


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
  ::ex42::ConstExampleWithNamespace* m_ref;
  std::vector<::ex42::ConstExampleWithNamespace>* m_refs;


};
} // namespace ex42


#endif
