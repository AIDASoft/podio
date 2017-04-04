#ifndef ConstExampleWithVectorMember_H
#define ConstExampleWithVectorMember_H
#include "ExampleWithVectorMemberData.h"
#include <vector>
#include <vector>
#include "podio/ObjectID.h"

//forward declarations


#include "ExampleWithVectorMemberObj.h"



class ExampleWithVectorMemberObj;
class ExampleWithVectorMember;
class ExampleWithVectorMemberCollection;
class ExampleWithVectorMemberCollectionIterator;

/** @class ConstExampleWithVectorMember
 *  Type with a vector member
 *  @author: B. Hegner
 */

class ConstExampleWithVectorMember {

  friend ExampleWithVectorMember;
  friend ExampleWithVectorMemberCollection;
  friend ExampleWithVectorMemberCollectionIterator;

public:

  /// default constructor
  ConstExampleWithVectorMember();
  
  /// constructor from existing ExampleWithVectorMemberObj
  ConstExampleWithVectorMember(ExampleWithVectorMemberObj* obj);
  /// copy constructor
  ConstExampleWithVectorMember(const ConstExampleWithVectorMember& other);
  /// copy-assignment operator
  ConstExampleWithVectorMember& operator=(const ConstExampleWithVectorMember& other);
  /// support cloning (deep-copy)
  ConstExampleWithVectorMember clone() const;
  /// destructor
  ~ConstExampleWithVectorMember();


public:


  unsigned int count_size() const;
  int count(unsigned int) const;
  std::vector<int>::const_iterator count_begin() const;
  std::vector<int>::const_iterator count_end() const;


  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleWithVectorMemberObj instance
  void unlink(){m_obj = nullptr;}

  bool operator==(const ConstExampleWithVectorMember& other) const {
       return (m_obj==other.m_obj);
  }

  bool operator==(const ExampleWithVectorMember& other) const;

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleWithVectorMember& p1,
//       const ExampleWithVectorMember& p2 );
  bool operator<(const ConstExampleWithVectorMember& other) const { return m_obj < other.m_obj  ; }

  unsigned int id() const { return getObjectID().collectionID * 10000000 + getObjectID().index  ;  } 

  const podio::ObjectID getObjectID() const;

private:
  ExampleWithVectorMemberObj* m_obj;

};


#endif
