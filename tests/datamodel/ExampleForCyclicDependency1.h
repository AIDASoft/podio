#ifndef ExampleForCyclicDependency1_H
#define ExampleForCyclicDependency1_H
#include "ExampleForCyclicDependency1Data.h"
#include <vector>
#include <iostream>
#include <iomanip>
#include "podio/ObjectID.h"

//forward declarations
class ExampleForCyclicDependency2;
class ConstExampleForCyclicDependency2;


#include "ExampleForCyclicDependency1Const.h"
#include "ExampleForCyclicDependency1Obj.h"



class ExampleForCyclicDependency1Collection;
class ExampleForCyclicDependency1CollectionIterator;
class ConstExampleForCyclicDependency1;

/** @class ExampleForCyclicDependency1
 *  Type for cyclic dependency
 *  @author: Benedikt Hegner
 */
class ExampleForCyclicDependency1 {

  friend ExampleForCyclicDependency1Collection;
  friend ExampleForCyclicDependency1CollectionIterator;
  friend ConstExampleForCyclicDependency1;

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

  /// conversion to const object
  operator ConstExampleForCyclicDependency1 () const;

public:

  /// Access the  a ref
  const ::ConstExampleForCyclicDependency2 ref() const;

  /// Set the  a ref
  void ref(::ConstExampleForCyclicDependency2 value);



  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleForCyclicDependency1Obj instance
  void unlink(){m_obj = nullptr;}

  bool operator==(const ExampleForCyclicDependency1& other) const {
    return (m_obj==other.m_obj);
  }

  bool operator==(const ConstExampleForCyclicDependency1& other) const;

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleForCyclicDependency1& p1,
//       const ExampleForCyclicDependency1& p2 );
  bool operator<(const ExampleForCyclicDependency1& other) const { return m_obj < other.m_obj  ; }


  unsigned int id() const { return getObjectID().collectionID * 10000000 + getObjectID().index  ;  } 

  const podio::ObjectID getObjectID() const;

private:
  ExampleForCyclicDependency1Obj* m_obj;

};

std::ostream& operator<<( std::ostream& o,const ConstExampleForCyclicDependency1& value );




#endif
