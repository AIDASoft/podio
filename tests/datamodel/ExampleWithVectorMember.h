#ifndef ExampleWithVectorMember_H
#define ExampleWithVectorMember_H
#include "ExampleWithVectorMemberData.h"
#include <vector>
#include <vector>
#include "podio/ObjectID.h"

// Type with a vector member
// author: B. Hegner

//forward declarations


#include "ExampleWithVectorMemberObj.h"



class ExampleWithVectorMemberCollection;
class ExampleWithVectorMemberCollectionIterator;

class ExampleWithVectorMember {

  friend ExampleWithVectorMemberCollection;
  friend ExampleWithVectorMemberCollectionIterator;

public:

  /// default constructor
  ExampleWithVectorMember();

  /// constructor from existing ExampleWithVectorMemberObj
  ExampleWithVectorMember(ExampleWithVectorMemberObj* obj);
  /// copy constructor
  ExampleWithVectorMember(const ExampleWithVectorMember& other);
  /// copy-assignment operator
  ExampleWithVectorMember& operator=(const ExampleWithVectorMember& other);
  /// support cloning (deep-copy)
  ExampleWithVectorMember clone() const;
  /// destructor
  ~ExampleWithVectorMember();

public:



  void addcount(int);
  unsigned int count_size() const;
  int count(unsigned int) const;
  std::vector<int>::const_iterator count_begin() const;
  std::vector<int>::const_iterator count_end() const;



  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleWithVectorMemberObj instance
  void unlink(){m_obj = nullptr;}

  bool operator==(const ExampleWithVectorMember& other) const {
    return (m_obj==other.m_obj);
  }

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleWithVectorMember& p1,
//       const ExampleWithVectorMember& p2 );
  bool operator<(const ExampleWithVectorMember& other) const { return m_obj < other.m_obj  ; }

  const podio::ObjectID getObjectID() const;

private:
  ExampleWithVectorMemberObj* m_obj;

};



#endif
