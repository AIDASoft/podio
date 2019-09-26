#ifndef EventInfoOBJ_H
#define EventInfoOBJ_H

// std includes
#include <atomic>
#include <iostream>

// data model specific includes
#include "podio/ObjBase.h"
#include "EventInfoData.h"



// forward declarations




class EventInfo;
class ConstEventInfo;


class EventInfoObj : public podio::ObjBase {
public:
  /// constructor
  EventInfoObj();
  /// copy constructor (does a deep-copy of relation containers)
  EventInfoObj(const EventInfoObj&);
  /// constructor from ObjectID and EventInfoData
  /// does not initialize the internal relation containers
  EventInfoObj(const podio::ObjectID id, EventInfoData data);
  virtual ~EventInfoObj();

public:
  EventInfoData data;


};



#endif
