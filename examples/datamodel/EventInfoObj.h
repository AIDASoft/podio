#ifndef EventInfoOBJ_H
#define EventInfoOBJ_H

// std includes
#include <atomic>
#include <iostream>

// data model specific includes
#include "albers/ObjBase.h"
#include "EventInfoData.h"



// forward declarations
class EventInfo;

class EventInfoObj : public albers::ObjBase {
public:
  /// constructor
  EventInfoObj();
  /// copy constructor (does a deep-copy of relation containers)
  EventInfoObj(const EventInfoObj&);
  /// constructor from ObjectID and EventInfoData
  /// does not initialize the internal relation containers
  EventInfoObj(const albers::ObjectID id, EventInfoData data);
  virtual ~EventInfoObj();

public:
  EventInfoData data;


};


#endif
