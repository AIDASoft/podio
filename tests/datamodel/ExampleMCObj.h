#ifndef ExampleMCOBJ_H
#define ExampleMCOBJ_H

// std includes
#include <atomic>
#include <iostream>

// data model specific includes
#include "ExampleMCData.h"
#include "podio/ObjBase.h"

#include <vector>

// forward declarations

class ExampleMC;
class ConstExampleMC;

class ExampleMCObj : public podio::ObjBase {
public:
  /// constructor
  ExampleMCObj();
  /// copy constructor (does a deep-copy of relation containers)
  ExampleMCObj(const ExampleMCObj &);
  /// constructor from ObjectID and ExampleMCData
  /// does not initialize the internal relation containers
  ExampleMCObj(const podio::ObjectID id, ExampleMCData data);
  virtual ~ExampleMCObj();

public:
  ExampleMCData data;
  std::vector<ConstExampleMC> *m_parents;
  std::vector<ConstExampleMC> *m_daughters;
};

#endif
