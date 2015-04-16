#ifndef EventInfoOBJ_H
#define EventInfoOBJ_H

// std includes
#include <atomic>
#include <iostream>

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
    if (id.index != albers::ObjectID::untracked){ return 1;};
    if (--ref_counter == 0) {
      std::cout << "deleting free-floating EventInfo at " << this << std::endl;
      delete this;
    }
    return 0;
    //return  ( (id.index == albers::ObjectID::untracked) ? --ref_counter : 1 );
  }; // returns current count

public:
  EventInfoData data;

  albers::ObjectID id;

private:
  std::atomic<int> ref_counter;
};


#endif
