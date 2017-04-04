#ifndef ConstEventInfo_H
#define ConstEventInfo_H
#include "EventInfoData.h"
#include <vector>
#include "podio/ObjectID.h"

//forward declarations


#include "EventInfoObj.h"



class EventInfoObj;
class EventInfo;
class EventInfoCollection;
class EventInfoCollectionIterator;

/** @class ConstEventInfo
 *  Event info
 *  @author: B. Hegner
 */

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

  /// Access the  event number
  const int& Number() const;


int getNumber() const; 
  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from EventInfoObj instance
  void unlink(){m_obj = nullptr;}

  bool operator==(const ConstEventInfo& other) const {
       return (m_obj==other.m_obj);
  }

  bool operator==(const EventInfo& other) const;

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const EventInfo& p1,
//       const EventInfo& p2 );
  bool operator<(const ConstEventInfo& other) const { return m_obj < other.m_obj  ; }

  unsigned int id() const { return getObjectID().collectionID * 10000000 + getObjectID().index  ;  } 

  const podio::ObjectID getObjectID() const;

private:
  EventInfoObj* m_obj;

};


#endif
