#ifndef ExampleWithArrayOBJ_H
#define ExampleWithArrayOBJ_H

// std includes
#include <atomic>
#include <iostream>

// data model specific includes
#include "ExampleWithArrayData.h"
#include "podio/ObjBase.h"

// forward declarations

class ExampleWithArray;
class ConstExampleWithArray;

class ExampleWithArrayObj : public podio::ObjBase {
public:
  /// constructor
  ExampleWithArrayObj();
  /// copy constructor (does a deep-copy of relation containers)
  ExampleWithArrayObj(const ExampleWithArrayObj &);
  /// constructor from ObjectID and ExampleWithArrayData
  /// does not initialize the internal relation containers
  ExampleWithArrayObj(const podio::ObjectID id, ExampleWithArrayData data);
  virtual ~ExampleWithArrayObj();

public:
  ExampleWithArrayData data;
};

#endif
