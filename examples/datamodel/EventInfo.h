#ifndef EventInfo_H
#define EventInfo_H
#include "EventInfoData.h"

#include <vector>
#include "albers/ObjectID.h"

// event number
// author: B. Hegner

//forward declarations
class EventInfoCollection;
class EventInfoCollectionIterator;
class EventInfoObj;

#include "EventInfoObj.h"

class EventInfo {

  friend EventInfoCollection;
  friend EventInfoCollectionIterator;

public:

  /// default constructor
  EventInfo();
  /// copy constructor
  EventInfo(const EventInfo& other);
  /// copy-assignment operator
  EventInfo& operator=(const EventInfo& other);
  /// constructor from existing EventInfoObj
  EventInfo(EventInfoObj* obj);
  /// support cloning (deep-copy)
  EventInfo clone() const;
  /// destructor
  ~EventInfo();

  const int& Number() const { return m_obj->data.Number; };

  void Number(int value) { m_obj->data.Number = value; };


  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from EventInfoObj instance
  void unlink(){m_obj = nullptr;};

  bool operator==(const EventInfo& other) const {
       return (m_obj==other.m_obj);
  }

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const EventInfo& p1,
//       const EventInfo& p2 );

  const albers::ObjectID getObjectID() const;

private:
  EventInfoObj* m_obj;

};

#endif
