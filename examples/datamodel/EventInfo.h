#ifndef EventInfo_H
#define EventInfo_H
#include "EventInfoData.h"

#include <vector>
#include "albers/ObjectID.h"
#include <iostream>

// event number
// author: B. Hegner

//forward declaration of EventInfo container
class EventInfoCollection;
class EventInfoCollectionIterator;

#include "EventInfoEntry.h"

namespace albers {
  class Registry;
}

class EventInfo {

  friend EventInfoCollection;
  friend EventInfoCollectionIterator;

public:

  EventInfo() : m_entry(new EventInfoEntry()){};
  EventInfo(const EventInfo& other) : m_entry(other.m_entry) {m_entry->increaseRefCount();};
  EventInfo& operator=(const EventInfo& other);
  EventInfo(EventInfoEntry* entry);
  ~EventInfo();

  const int& Number() const;

  void Number(int value);


  bool isAvailable() const; // precheck whether the pointee actually exists
  void unlink(){m_entry = nullptr;};

  bool operator==(const EventInfo& other) const {
       return (m_entry==other.m_entry);
  }

  /// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const EventInfo& p1,
//       const EventInfo& p2 );

  const albers::ObjectID getObjectID() const;

private:
  EventInfoEntry* m_entry;

};

#endif
