#ifndef EventInfo_H
#define EventInfo_H
#include "EventInfoData.h"
#include "SimpleStruct.h"

#include <vector>
#include "podio/ObjectID.h"

//  a simple struct
// author: B. Hegner

//forward declarations
class EventInfoCollection;
class EventInfoCollectionIterator;
class ConstEventInfo;


#include "EventInfoConst.h"
#include "EventInfoObj.h"

class EventInfo {

  friend EventInfoCollection;
  friend EventInfoCollectionIterator;
  friend ConstEventInfo;

public:

  /// default constructor
  EventInfo();
    EventInfo(int Number,SimpleStruct simple);

  /// constructor from existing EventInfoObj
  EventInfo(EventInfoObj* obj);
  /// copy constructor
  EventInfo(const EventInfo& other);
  /// copy-assignment operator
  EventInfo& operator=(const EventInfo& other);
  /// support cloning (deep-copy)
  EventInfo clone() const;
  /// destructor
  ~EventInfo();

  /// conversion to const object
  operator ConstEventInfo () const;

public:

  const int& Number() const { return m_obj->data.Number; };
  const SimpleStruct& simple() const { return m_obj->data.simple; };

  void Number(int value) { m_obj->data.Number = value; };
  SimpleStruct& simple() { return m_obj->data.simple; };
  void simple(class SimpleStruct value);


  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from EventInfoObj instance
  void unlink(){m_obj = nullptr;};

  bool operator==(const EventInfo& other) const {
       return (m_obj==other.m_obj);
  }

  bool operator==(const ConstEventInfo& other) const;

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const EventInfo& p1,
//       const EventInfo& p2 );

  const podio::ObjectID getObjectID() const;

private:
  EventInfoObj* m_obj;

};

#endif
