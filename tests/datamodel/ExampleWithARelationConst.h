#ifndef ConstExampleWithARelation_H
#define ConstExampleWithARelation_H
#include "ExampleWithARelationData.h"
#include <vector>
#include "ExampleWithNamespace.h"
#include <vector>
#include "podio/ObjectID.h"

//forward declarations
namespace ex42 {
class ExampleWithNamespace;
class ConstExampleWithNamespace;
}


#include "ExampleWithARelationObj.h"

namespace ex42 {

class ExampleWithARelationObj;
class ExampleWithARelation;
class ExampleWithARelationCollection;
class ExampleWithARelationCollectionIterator;

/** @class ConstExampleWithARelation
 *  Type with namespace and namespaced relation
 *  @author: Joschka Lingemann
 */

class ConstExampleWithARelation {

  friend ExampleWithARelation;
  friend ExampleWithARelationCollection;
  friend ExampleWithARelationCollectionIterator;

public:

  /// default constructor
  ConstExampleWithARelation();
  ConstExampleWithARelation(float number);

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

  /// Access the  just a number
  const float& number() const;
  /// Access the  a ref in a namespace
  const ex42::ConstExampleWithNamespace ref() const;

  unsigned int refs_size() const;
  ex42::ConstExampleWithNamespace refs(unsigned int) const;
  std::vector<ex42::ConstExampleWithNamespace>::const_iterator refs_begin() const;
  std::vector<ex42::ConstExampleWithNamespace>::const_iterator refs_end() const;


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

  unsigned int id() const { return getObjectID().collectionID * 10000000 + getObjectID().index  ;  } 

  const podio::ObjectID getObjectID() const;

private:
  ExampleWithARelationObj* m_obj;

};
} // namespace ex42

#endif
