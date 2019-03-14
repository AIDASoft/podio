#ifndef ExampleWithNamespaceOBJ_H
#define ExampleWithNamespaceOBJ_H

// std includes
#include <atomic>
#include <iostream>

// data model specific includes
#include "ExampleWithNamespaceData.h"
#include "podio/ObjBase.h"

// forward declarations

namespace ex {

class ExampleWithNamespace;
class ConstExampleWithNamespace;

class ExampleWithNamespaceObj : public podio::ObjBase {
public:
  /// constructor
  ExampleWithNamespaceObj();
  /// copy constructor (does a deep-copy of relation containers)
  ExampleWithNamespaceObj(const ExampleWithNamespaceObj &);
  /// constructor from ObjectID and ExampleWithNamespaceData
  /// does not initialize the internal relation containers
  ExampleWithNamespaceObj(const podio::ObjectID id,
                          ExampleWithNamespaceData data);
  virtual ~ExampleWithNamespaceObj();

public:
  ExampleWithNamespaceData data;
};
} // namespace ex

#endif
