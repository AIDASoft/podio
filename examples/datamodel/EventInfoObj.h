#ifndef EventInfoOBJ_H
#define EventInfoOBJ_H

// std includes
#include <atomic>

// data model specific includes
#include "albers/ObjectID.h"
#include "EventInfoData.h"



// forward declarations
class EventInfo;

class EventInfoObj {
public:
  EventInfoObj();
  EventInfoObj(const EventInfoObj&); //TODO: deep copy!
  EventInfoObj(const albers::ObjectID id, EventInfoData data);
  ~EventInfoObj();
  void increaseRefCount() {
    if (id.index == albers::ObjectID::untracked) ++ref_counter;
  };

  int decreaseRefCount(){
    return  ( (id.index == albers::ObjectID::untracked) ? --ref_counter : 1 );
  }; // returns current count

public:
  EventInfoData data;

  albers::ObjectID id;
  std::atomic<int> ref_counter;
};


#endif
