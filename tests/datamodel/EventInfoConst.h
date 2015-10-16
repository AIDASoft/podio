#ifndef ConstEventInfo_H
#define ConstEventInfo_H
#include "EventInfoData.h"

#include <vector>
#include "podio/ObjectID.h"

//  event number
// author: B. Hegner

//forward declarations
class EventInfoCollection;
class EventInfoCollectionIterator;


#include "EventInfoObj.h"

class ConstEventInfo {

  friend EventInfo;
  friend EventInfoCollection;
  friend EventInfoCollectionIterator;

public:

  /// default constructor
  ConstEventInfo();
  ConstEventInfo(int Number);

  /// constructor from existing EventInfoObj
  ConstEventInfo(EventInfoObj* obj);
  /// copy constructor
  ConstEventInfo(const ConstEventInfo& other);
  /// copy-assignment operator
  ConstEventInfo& operator=(const ConstEventInfo& other);
  /// support cloning (deep-copy)
  ConstEventInfo clone() const;
  /// destructor
  ~ConstEventInfo();


public:

  const int& Number() const { return m_obj->data.Number; };


  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from EventInfoObj instance
  void unlink(){m_obj = nullptr;};

  bool operator==(const ConstEventInfo& other) const {
       return (m_obj==other.m_obj);
  }

  bool operator==(const EventInfo& other) const;

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const EventInfo& p1,
//       const EventInfo& p2 );

  const podio::ObjectID getObjectID() const;

private:
  EventInfoObj* m_obj;

};

#endif
