#ifndef ExampleWithNamespace_H
#define ExampleWithNamespace_H
#include "NamespaceStruct.h"
#include "ExampleWithNamespaceData.h"
#include <vector>
#include "podio/ObjectID.h"

// Type with namespace and namespaced member
// author: Joschka Lingemann

//forward declarations


#include "ExampleWithNamespaceConst.h"
#include "ExampleWithNamespaceObj.h"

namespace ex {

class ExampleWithNamespaceCollection;
class ExampleWithNamespaceCollectionIterator;
class ConstExampleWithNamespace;

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

  const ex2::NamespaceStruct& data() const;

  ex2::NamespaceStruct& data();
  void data(class ex2::NamespaceStruct value);



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

  const podio::ObjectID getObjectID() const;

private:
  ExampleWithNamespaceObj* m_obj;

};

} // namespace ex

#endif
