#ifndef EventInfo_H
#define EventInfo_H
#include "EventInfoData.h"
#include <vector>
#include "podio/ObjectID.h"

// Event info
// author: B. Hegner

//forward declarations


#include "EventInfoConst.h"
#include "EventInfoObj.h"



class EventInfoCollection;
class EventInfoCollectionIterator;
class ConstEventInfo;

class EventInfo {

  friend EventInfoCollection;
  friend EventInfoCollectionIterator;
  friend ConstEventInfo;

public:

  /// default constructor
  EventInfo();
  EventInfo(int Number);

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

  const int& Number() const;

  void Number(int value);



void setNumber(int n) { Number( n ) ; } 
int getNumber() const; 
  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from EventInfoObj instance
  void unlink(){m_obj = nullptr;}

  bool operator==(const EventInfo& other) const {
    return (m_obj==other.m_obj);
  }

  bool operator==(const ConstEventInfo& other) const;

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const EventInfo& p1,
//       const EventInfo& p2 );
  bool operator<(const EventInfo& other) const { return m_obj < other.m_obj  ; }

  const podio::ObjectID getObjectID() const;

private:
  EventInfoObj* m_obj;

};



#endif
