#ifndef ExampleForCyclicDependency1_H
#define ExampleForCyclicDependency1_H
#include "ExampleForCyclicDependency1Data.h"
#include <vector>
#include "podio/ObjectID.h"

//forward declarations
class ExampleForCyclicDependency2;


#include "ExampleForCyclicDependency1Obj.h"



class ExampleForCyclicDependency1Collection;
class ExampleForCyclicDependency1CollectionIterator;

/** @class ExampleForCyclicDependency1
 *  Type for cyclic dependency
 *  @author: Benedikt Hegner
 */
class ExampleForCyclicDependency1 {

  friend ExampleForCyclicDependency1Collection;
  friend ExampleForCyclicDependency1CollectionIterator;

public:

  /// default constructor
  ExampleForCyclicDependency1();

  /// constructor from existing ExampleForCyclicDependency1Obj
  ExampleForCyclicDependency1(ExampleForCyclicDependency1Obj* obj);
  /// copy constructor
  ExampleForCyclicDependency1(const ExampleForCyclicDependency1& other);
  /// copy-assignment operator
  ExampleForCyclicDependency1& operator=(const ExampleForCyclicDependency1& other);
  /// support cloning (deep-copy)
  ExampleForCyclicDependency1 clone() const;
  /// destructor
  ~ExampleForCyclicDependency1();

public:

  /// Access the  a ref
  const ::ExampleForCyclicDependency2 ref() const;

  /// Set the  a ref
  void ref(::ExampleForCyclicDependency2 value);



  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleForCyclicDependency1Obj instance
  void unlink(){m_obj = nullptr;}

  bool operator==(const ExampleForCyclicDependency1& other) const {
    return (m_obj==other.m_obj);
  }

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleForCyclicDependency1& p1,
//       const ExampleForCyclicDependency1& p2 );
  bool operator<(const ExampleForCyclicDependency1& other) const { return m_obj < other.m_obj  ; }

  const podio::ObjectID getObjectID() const;

private:
  ExampleForCyclicDependency1Obj* m_obj;

};



#endif
