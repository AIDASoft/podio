#ifndef EventInfo_H
#define EventInfo_H
#include "EventInfoData.h"

#include <vector>
#include "albers/ObjectID.h"
#include <iostream>

// event number
// author: B. Hegner

//forward declarations
class EventInfoCollection;
class EventInfoCollectionIterator;
class EventInfoObj;

#include "EventInfoObj.h"

namespace albers {
  class Registry;
}


class EventInfo {

  friend EventInfoCollection;
  friend EventInfoCollectionIterator;

public:

  EventInfo();
  EventInfo(const EventInfo& other);
  EventInfo& operator=(const EventInfo& other);
  EventInfo(EventInfoObj* obj);
  ~EventInfo();

  const int& Number() const;

  void Number(int value);


  bool isAvailable() const; // precheck whether the pointee actually exists
  void unlink(){m_obj = nullptr;};

  bool operator==(const EventInfo& other) const {
       return (m_obj==other.m_obj);
  }

  /// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const EventInfo& p1,
//       const EventInfo& p2 );

  const albers::ObjectID getObjectID() const;

private:
  EventInfoObj* m_obj;

};

#endif
