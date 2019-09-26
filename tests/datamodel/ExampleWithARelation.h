#ifndef ExampleWithARelation_H
#define ExampleWithARelation_H
#include "ExampleWithARelationData.h"
#include <vector>
#include "ExampleWithNamespace.h"
#include <vector>
#include <iostream>
#include <iomanip>
#include "podio/ObjectID.h"

//forward declarations
namespace ex42 {
class ExampleWithNamespace;
class ConstExampleWithNamespace;
}


#include "ExampleWithARelationConst.h"
#include "ExampleWithARelationObj.h"

namespace ex42 {

class ExampleWithARelationCollection;
class ExampleWithARelationCollectionIterator;
class ConstExampleWithARelation;

/** @class ExampleWithARelation
 *  Type with namespace and namespaced relation
 *  @author: Joschka Lingemann
 */
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

  /// Access the  just a number
  const float& number() const;
  /// Access the  a ref in a namespace
  const ex42::ConstExampleWithNamespace ref() const;

  /// Set the  just a number
  void number(float value);

  /// Set the  a ref in a namespace
  void ref(ex42::ConstExampleWithNamespace value);

  void addrefs(ex42::ConstExampleWithNamespace);
  unsigned int refs_size() const;
  ex42::ConstExampleWithNamespace refs(unsigned int) const;
  std::vector<ex42::ConstExampleWithNamespace>::const_iterator refs_begin() const;
  std::vector<ex42::ConstExampleWithNamespace>::const_iterator refs_end() const;



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


  unsigned int id() const { return getObjectID().collectionID * 10000000 + getObjectID().index  ;  } 

  const podio::ObjectID getObjectID() const;

private:
  ExampleWithARelationObj* m_obj;

};

std::ostream& operator<<( std::ostream& o,const ConstExampleWithARelation& value );


} // namespace ex42

#endif
