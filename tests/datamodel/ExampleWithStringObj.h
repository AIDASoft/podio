#ifndef ExampleWithStringOBJ_H
#define ExampleWithStringOBJ_H

// std includes
#include <atomic>
#include <iostream>

// data model specific includes
#include "ExampleWithStringData.h"
#include "podio/ObjBase.h"

// forward declarations

class ExampleWithString;
class ConstExampleWithString;

class ExampleWithStringObj : public podio::ObjBase {
public:
  /// constructor
  ExampleWithStringObj();
  /// copy constructor (does a deep-copy of relation containers)
  ExampleWithStringObj(const ExampleWithStringObj &);
  /// constructor from ObjectID and ExampleWithStringData
  /// does not initialize the internal relation containers
  ExampleWithStringObj(const podio::ObjectID id, ExampleWithStringData data);
  virtual ~ExampleWithStringObj();

public:
  ExampleWithStringData data;
};

#endif
