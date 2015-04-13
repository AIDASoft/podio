#ifndef EventInfoENTRY_H
#define EventInfoENTRY_H

// std includes
#include <atomic>

// datamodel specific includes
#include "albers/ObjectID.h"
#include "EventInfoData.h"



class EventInfoEntry {
public:
  EventInfoEntry();
  EventInfoEntry(const EventInfoEntry&); //TODO: deep copy!
  EventInfoEntry(const albers::ObjectID id, EventInfoData data);
  ~EventInfoEntry();
  void increaseRefCount() {
    if (id.index == albers::ObjectID::untracked) ++ref_counter;
  }
;
  int decreaseRefCount(){
    return  ( (id.index == albers::ObjectID::untracked) ? --ref_counter : 1 );
  }; // returns current count

public:
  EventInfoData data;

  albers::ObjectID id;
  std::atomic<int> ref_counter;
};


#endif
