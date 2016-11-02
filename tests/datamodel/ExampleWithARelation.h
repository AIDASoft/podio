#ifndef ExampleWithARelation_H
#define ExampleWithARelation_H
#include "ExampleWithARelationData.h"
#include <vector>
#include "ExampleWithNamespace.h"
#include <vector>
#include "podio/ObjectID.h"

//forward declarations
namespace ex {
class ExampleWithNamespace;
}


#include "ExampleWithARelationObj.h"

namespace ex {

class ExampleWithARelationCollection;
class ExampleWithARelationCollectionIterator;

/** @class ExampleWithARelation
 *  Type with namespace and namespaced relation
 *  @author: Joschka Lingemann
 */
class ExampleWithARelation {

  friend ExampleWithARelationCollection;
  friend ExampleWithARelationCollectionIterator;

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

public:

  /// Access the  just a number
  const float& number() const;
  /// Access the  a ref in a namespace
  const ex::ExampleWithNamespace ref() const;

  /// Set the  just a number
  void number(float value);

  /// Set the  a ref in a namespace
  void ref(ex::ExampleWithNamespace value);

  void addrefs(ex::ExampleWithNamespace);
  unsigned int refs_size() const;
  ex::ExampleWithNamespace refs(unsigned int) const;
  std::vector<ex::ExampleWithNamespace>::const_iterator refs_begin() const;
  std::vector<ex::ExampleWithNamespace>::const_iterator refs_end() const;



  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleWithARelationObj instance
  void unlink(){m_obj = nullptr;}

  bool operator==(const ExampleWithARelation& other) const {
    return (m_obj==other.m_obj);
  }

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
