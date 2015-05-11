#ifndef ExampleWithVectorMember_H
#define ExampleWithVectorMember_H
#include "ExampleWithVectorMemberData.h"
#include <vector>

#include <vector>
#include "podio/ObjectID.h"

//  various ADC counts
// author: B. Hegner

//forward declarations
class ExampleWithVectorMemberCollection;
class ExampleWithVectorMemberCollectionIterator;
class ExampleWithVectorMemberObj;

#include "ExampleWithVectorMemberObj.h"

class ExampleWithVectorMember {

  friend ExampleWithVectorMemberCollection;
  friend ExampleWithVectorMemberCollectionIterator;

public:

  /// default constructor
  ExampleWithVectorMember();
    ExampleWithVectorMember(std::vector<int> count);

  /// copy constructor
  ExampleWithVectorMember(const ExampleWithVectorMember& other);
  /// copy-assignment operator
  ExampleWithVectorMember& operator=(const ExampleWithVectorMember& other);
  /// constructor from existing ExampleWithVectorMemberObj
  ExampleWithVectorMember(ExampleWithVectorMemberObj* obj);
  /// support cloning (deep-copy)
  ExampleWithVectorMember clone() const;
  /// destructor
  ~ExampleWithVectorMember();

  const std::vector<int>& count() const { return m_obj->data.count; };
  std::vector<int>& count() { return m_obj->data.count; };

  void count(class std::vector<int> value);


  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleWithVectorMemberObj instance
  void unlink(){m_obj = nullptr;};

  bool operator==(const ExampleWithVectorMember& other) const {
       return (m_obj==other.m_obj);
  }

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleWithVectorMember& p1,
//       const ExampleWithVectorMember& p2 );

  const podio::ObjectID getObjectID() const;

private:
  ExampleWithVectorMemberObj* m_obj;

};

#endif
