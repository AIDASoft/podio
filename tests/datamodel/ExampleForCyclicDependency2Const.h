#ifndef ConstExampleForCyclicDependency2_H
#define ConstExampleForCyclicDependency2_H
#include "ExampleForCyclicDependency2Data.h"

#include <vector>
#include "podio/ObjectID.h"

// Type for cyclic dependency
// author: Benedikt Hegner

//forward declarations
class ExampleForCyclicDependency2Collection;
class ExampleForCyclicDependency2CollectionIterator;
class ExampleForCyclicDependency1;
class ConstExampleForCyclicDependency1;


#include "ExampleForCyclicDependency2Obj.h"

class ConstExampleForCyclicDependency2 {

  friend ExampleForCyclicDependency2;
  friend ExampleForCyclicDependency2Collection;
  friend ExampleForCyclicDependency2CollectionIterator;

public:

  /// default constructor
  ConstExampleForCyclicDependency2();
  
  /// constructor from existing ExampleForCyclicDependency2Obj
  ConstExampleForCyclicDependency2(ExampleForCyclicDependency2Obj* obj);
  /// copy constructor
  ConstExampleForCyclicDependency2(const ConstExampleForCyclicDependency2& other);
  /// copy-assignment operator
  ConstExampleForCyclicDependency2& operator=(const ConstExampleForCyclicDependency2& other);
  /// support cloning (deep-copy)
  ConstExampleForCyclicDependency2 clone() const;
  /// destructor
  ~ConstExampleForCyclicDependency2();


public:

  const ConstExampleForCyclicDependency1 ref() const;


  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleForCyclicDependency2Obj instance
  void unlink(){m_obj = nullptr;};

  bool operator==(const ConstExampleForCyclicDependency2& other) const {
       return (m_obj==other.m_obj);
  }

  bool operator==(const ExampleForCyclicDependency2& other) const;

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleForCyclicDependency2& p1,
//       const ExampleForCyclicDependency2& p2 );

  const podio::ObjectID getObjectID() const;

private:
  ExampleForCyclicDependency2Obj* m_obj;

};

#endif
