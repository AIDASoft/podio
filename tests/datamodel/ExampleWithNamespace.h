#ifndef ExampleWithNamespace_H
#define ExampleWithNamespace_H
#include "NamespaceStruct.h"
#include "ExampleWithNamespaceData.h"
#include <vector>
#include <iostream>
#include <iomanip>
#include "podio/ObjectID.h"

//forward declarations


#include "ExampleWithNamespaceConst.h"
#include "ExampleWithNamespaceObj.h"

namespace ex42 {

class ExampleWithNamespaceCollection;
class ExampleWithNamespaceCollectionIterator;
class ConstExampleWithNamespace;

/** @class ExampleWithNamespace
 *  Type with namespace and namespaced member
 *  @author: Joschka Lingemann
 */
class ExampleWithNamespace {

  friend ExampleWithNamespaceCollection;
  friend ExampleWithNamespaceCollectionIterator;
  friend ConstExampleWithNamespace;

public:

  /// default constructor
  ExampleWithNamespace();
  ExampleWithNamespace(ex2::NamespaceStruct data);

  /// constructor from existing ExampleWithNamespaceObj
  ExampleWithNamespace(ExampleWithNamespaceObj* obj);
  /// copy constructor
  ExampleWithNamespace(const ExampleWithNamespace& other);
  /// copy-assignment operator
  ExampleWithNamespace& operator=(const ExampleWithNamespace& other);
  /// support cloning (deep-copy)
  ExampleWithNamespace clone() const;
  /// destructor
  ~ExampleWithNamespace();

  /// conversion to const object
  operator ConstExampleWithNamespace () const;

public:

  /// Access the  a component
  const ex2::NamespaceStruct& data() const;
  /// Access the member of  a component
  const int& x() const;
  /// Access the member of  a component
  const int& y() const;

  /// Get reference to the  a component
  ex2::NamespaceStruct& data();
  /// Set the  a component
  void data(class ex2::NamespaceStruct value);
  /// Set the  member of  a component
  void x(int value);

  /// Set the  member of  a component
  void y(int value);




  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleWithNamespaceObj instance
  void unlink(){m_obj = nullptr;}

  bool operator==(const ExampleWithNamespace& other) const {
    return (m_obj==other.m_obj);
  }

  bool operator==(const ConstExampleWithNamespace& other) const;

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleWithNamespace& p1,
//       const ExampleWithNamespace& p2 );
  bool operator<(const ExampleWithNamespace& other) const { return m_obj < other.m_obj  ; }


  unsigned int id() const { return getObjectID().collectionID * 10000000 + getObjectID().index  ;  } 

  const podio::ObjectID getObjectID() const;

private:
  ExampleWithNamespaceObj* m_obj;

};

std::ostream& operator<<( std::ostream& o,const ConstExampleWithNamespace& value );


} // namespace ex42

#endif
