#ifndef ConstExampleWithARelation_H
#define ConstExampleWithARelation_H
#include "ExampleWithARelationData.h"
#include <vector>
#include "podio/ObjectID.h"

// Type with namespace and namespaced relation
// author: Joschka Lingemann

//forward declarations
namespace ex {
class ExampleWithNamespace;
class ConstExampleWithNamespace;
}


#include "ExampleWithARelationObj.h"

namespace ex {

class ExampleWithARelationObj;
class ExampleWithARelation;
class ExampleWithARelationCollection;
class ExampleWithARelationCollectionIterator;

class ConstExampleWithARelation {

  friend ExampleWithARelation;
  friend ExampleWithARelationCollection;
  friend ExampleWithARelationCollectionIterator;

public:

  /// default constructor
  ConstExampleWithARelation();
  
  /// constructor from existing ExampleWithARelationObj
  ConstExampleWithARelation(ExampleWithARelationObj* obj);
  /// copy constructor
  ConstExampleWithARelation(const ConstExampleWithARelation& other);
  /// copy-assignment operator
  ConstExampleWithARelation& operator=(const ConstExampleWithARelation& other);
  /// support cloning (deep-copy)
  ConstExampleWithARelation clone() const;
  /// destructor
  ~ConstExampleWithARelation();


public:

  const ex::ConstExampleWithNamespace ref() const;



  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleWithARelationObj instance
  void unlink(){m_obj = nullptr;}

  bool operator==(const ConstExampleWithARelation& other) const {
       return (m_obj==other.m_obj);
  }

  bool operator==(const ExampleWithARelation& other) const;

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleWithARelation& p1,
//       const ExampleWithARelation& p2 );
  bool operator<(const ConstExampleWithARelation& other) const { return m_obj < other.m_obj  ; }

  const podio::ObjectID getObjectID() const;

private:
  ExampleWithARelationObj* m_obj;

};
} // namespace ex

#endif
