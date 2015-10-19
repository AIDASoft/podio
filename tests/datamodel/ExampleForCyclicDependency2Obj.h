#ifndef ExampleForCyclicDependency2OBJ_H
#define ExampleForCyclicDependency2OBJ_H

// std includes
#include <atomic>
#include <iostream>

// data model specific includes
#include "podio/ObjBase.h"
#include "ExampleForCyclicDependency2Data.h"



// forward declarations
class ExampleForCyclicDependency2;
class ConstExampleForCyclicDependency2;
class ConstExampleForCyclicDependency1;


class ExampleForCyclicDependency2Obj : public podio::ObjBase {
public:
  /// constructor
  ExampleForCyclicDependency2Obj();
  /// copy constructor (does a deep-copy of relation containers)
  ExampleForCyclicDependency2Obj(const ExampleForCyclicDependency2Obj&);
  /// constructor from ObjectID and ExampleForCyclicDependency2Data
  /// does not initialize the internal relation containers
  ExampleForCyclicDependency2Obj(const podio::ObjectID id, ExampleForCyclicDependency2Data data);
  virtual ~ExampleForCyclicDependency2Obj();

public:
  ExampleForCyclicDependency2Data data;
  ConstExampleForCyclicDependency1* m_ref;


};


#endif
