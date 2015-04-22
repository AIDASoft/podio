#ifndef ExampleWithVectorMemberOBJ_H
#define ExampleWithVectorMemberOBJ_H

// std includes
#include <atomic>
#include <iostream>

// data model specific includes
#include "albers/ObjBase.h"
#include "ExampleWithVectorMemberData.h"



// forward declarations
class ExampleWithVectorMember;

class ExampleWithVectorMemberObj : public albers::ObjBase {
public:
  /// constructor
  ExampleWithVectorMemberObj();
  /// copy constructor (does a deep-copy of relation containers)
  ExampleWithVectorMemberObj(const ExampleWithVectorMemberObj&);
  /// constructor from ObjectID and ExampleWithVectorMemberData
  /// does not initialize the internal relation containers
  ExampleWithVectorMemberObj(const albers::ObjectID id, ExampleWithVectorMemberData data);
  virtual ~ExampleWithVectorMemberObj();

public:
  ExampleWithVectorMemberData data;


};


#endif
