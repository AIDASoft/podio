#ifndef ConstExampleForCyclicDependency1_H
#define ConstExampleForCyclicDependency1_H
#include "ExampleForCyclicDependency1Data.h"

#include <vector>
#include "podio/ObjectID.h"

// Type for cyclic dependency
// author: Benedikt Hegner

//forward declarations
class ExampleForCyclicDependency2;
class ConstExampleForCyclicDependency2;


#include "ExampleForCyclicDependency1Obj.h"



class ExampleForCyclicDependency1Obj;
class ExampleForCyclicDependency1;
class ExampleForCyclicDependency1Collection;
class ExampleForCyclicDependency1CollectionIterator;

class ConstExampleForCyclicDependency1 {

  friend ExampleForCyclicDependency1;
  friend ExampleForCyclicDependency1Collection;
  friend ExampleForCyclicDependency1CollectionIterator;

public:

  /// default constructor
  ConstExampleForCyclicDependency1();
  
  /// constructor from existing ExampleForCyclicDependency1Obj
  ConstExampleForCyclicDependency1(ExampleForCyclicDependency1Obj* obj);
  /// copy constructor
  ConstExampleForCyclicDependency1(const ConstExampleForCyclicDependency1& other);
  /// copy-assignment operator
  ConstExampleForCyclicDependency1& operator=(const ConstExampleForCyclicDependency1& other);
  /// support cloning (deep-copy)
  ConstExampleForCyclicDependency1 clone() const;
  /// destructor
  ~ConstExampleForCyclicDependency1();


public:

  const ConstExampleForCyclicDependency2 ref() const;


  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleForCyclicDependency1Obj instance
  void unlink(){m_obj = nullptr;}

  bool operator==(const ConstExampleForCyclicDependency1& other) const {
       return (m_obj==other.m_obj);
  }

  bool operator==(const ExampleForCyclicDependency1& other) const;

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleForCyclicDependency1& p1,
//       const ExampleForCyclicDependency1& p2 );

  const podio::ObjectID getObjectID() const;

private:
  ExampleForCyclicDependency1Obj* m_obj;

};


#endif
