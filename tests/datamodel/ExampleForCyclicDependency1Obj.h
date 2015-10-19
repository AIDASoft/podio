#ifndef ExampleForCyclicDependency1OBJ_H
#define ExampleForCyclicDependency1OBJ_H

// std includes
#include <atomic>
#include <iostream>

// data model specific includes
#include "podio/ObjBase.h"
#include "ExampleForCyclicDependency1Data.h"



// forward declarations
class ExampleForCyclicDependency1;
class ConstExampleForCyclicDependency1;
class ConstExampleForCyclicDependency2;


class ExampleForCyclicDependency1Obj : public podio::ObjBase {
public:
  /// constructor
  ExampleForCyclicDependency1Obj();
  /// copy constructor (does a deep-copy of relation containers)
  ExampleForCyclicDependency1Obj(const ExampleForCyclicDependency1Obj&);
  /// constructor from ObjectID and ExampleForCyclicDependency1Data
  /// does not initialize the internal relation containers
  ExampleForCyclicDependency1Obj(const podio::ObjectID id, ExampleForCyclicDependency1Data data);
  virtual ~ExampleForCyclicDependency1Obj();

public:
  ExampleForCyclicDependency1Data data;
  ConstExampleForCyclicDependency2* m_ref;


};


#endif
