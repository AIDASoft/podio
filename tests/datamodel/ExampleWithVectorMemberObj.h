#ifndef ExampleWithVectorMemberOBJ_H
#define ExampleWithVectorMemberOBJ_H

// std includes
#include <atomic>
#include <iostream>

// data model specific includes
#include "podio/ObjBase.h"
#include "ExampleWithVectorMemberData.h"

#include <vector>


// forward declarations




class ExampleWithVectorMember;
class ConstExampleWithVectorMember;


class ExampleWithVectorMemberObj : public podio::ObjBase {
public:
  /// constructor
  ExampleWithVectorMemberObj();
  /// copy constructor (does a deep-copy of relation containers)
  ExampleWithVectorMemberObj(const ExampleWithVectorMemberObj&);
  /// constructor from ObjectID and ExampleWithVectorMemberData
  /// does not initialize the internal relation containers
  ExampleWithVectorMemberObj(const podio::ObjectID id, ExampleWithVectorMemberData data);
  virtual ~ExampleWithVectorMemberObj();

public:
  ExampleWithVectorMemberData data;
  std::vector<int>* m_count;


};



#endif
