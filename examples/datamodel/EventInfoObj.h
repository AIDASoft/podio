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
  EventInfoObj();
  EventInfoObj(const EventInfoObj&); //TODO: deep copy!
  EventInfoObj(const albers::ObjectID id, EventInfoData data);
  virtual ~EventInfoObj();

public:
  EventInfoData data;


};


#endif
