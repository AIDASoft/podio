#ifndef ExampleWithComponentOBJ_H
#define ExampleWithComponentOBJ_H

// std includes
#include <atomic>
#include <iostream>

// data model specific includes
#include "ExampleWithComponentData.h"
#include "podio/ObjBase.h"

// forward declarations

class ExampleWithComponent;
class ConstExampleWithComponent;

class ExampleWithComponentObj : public podio::ObjBase {
public:
  /// constructor
  ExampleWithComponentObj();
  /// copy constructor (does a deep-copy of relation containers)
  ExampleWithComponentObj(const ExampleWithComponentObj &);
  /// constructor from ObjectID and ExampleWithComponentData
  /// does not initialize the internal relation containers
  ExampleWithComponentObj(const podio::ObjectID id,
                          ExampleWithComponentData data);
  virtual ~ExampleWithComponentObj();

public:
  ExampleWithComponentData data;
};

#endif
