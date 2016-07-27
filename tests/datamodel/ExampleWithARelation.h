#ifndef ExampleWithARelation_H
#define ExampleWithARelation_H
#include "ExampleWithARelationData.h"
#include <vector>
#include "ExampleWithNamespace.h"
#include <vector>
#include "podio/ObjectID.h"

// Type with namespace and namespaced relation
// author: Joschka Lingemann

//forward declarations
namespace ex {
class ExampleWithNamespace;
class ConstExampleWithNamespace;
}


#include "ExampleWithARelationConst.h"
#include "ExampleWithARelationObj.h"

namespace ex {

class ExampleWithARelationCollection;
class ExampleWithARelationCollectionIterator;
class ConstExampleWithARelation;

class ExampleWithARelation {

  friend ExampleWithARelationCollection;
  friend ExampleWithARelationCollectionIterator;
  friend ConstExampleWithARelation;

public:

  /// default constructor
  ExampleWithARelation();
  ExampleWithARelation(float number);

  /// constructor from existing ExampleWithARelationObj
  ExampleWithARelation(ExampleWithARelationObj* obj);
  /// copy constructor
  ExampleWithARelation(const ExampleWithARelation& other);
  /// copy-assignment operator
  ExampleWithARelation& operator=(const ExampleWithARelation& other);
  /// support cloning (deep-copy)
  ExampleWithARelation clone() const;
  /// destructor
  ~ExampleWithARelation();

  /// conversion to const object
  operator ConstExampleWithARelation () const;

public:

  const float& number() const;
  const ex::ConstExampleWithNamespace ref() const;

  void number(float value);

  void ref(ex::ConstExampleWithNamespace value);

  void addrefs(ex::ConstExampleWithNamespace);
  unsigned int refs_size() const;
  ex::ConstExampleWithNamespace refs(unsigned int) const;
  std::vector<ex::ConstExampleWithNamespace>::const_iterator refs_begin() const;
  std::vector<ex::ConstExampleWithNamespace>::const_iterator refs_end() const;



  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleWithARelationObj instance
  void unlink(){m_obj = nullptr;}

  bool operator==(const ExampleWithARelation& other) const {
    return (m_obj==other.m_obj);
  }

  bool operator==(const ConstExampleWithARelation& other) const;

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleWithARelation& p1,
//       const ExampleWithARelation& p2 );
  bool operator<(const ExampleWithARelation& other) const { return m_obj < other.m_obj  ; }

  const podio::ObjectID getObjectID() const;

private:
  ExampleWithARelationObj* m_obj;

};

} // namespace ex

#endif
