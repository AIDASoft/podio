#ifndef ExampleForCyclicDependency2_H
#define ExampleForCyclicDependency2_H
#include "ExampleForCyclicDependency2Data.h"
#include <vector>
#include "podio/ObjectID.h"

//forward declarations
class ExampleForCyclicDependency1;


#include "ExampleForCyclicDependency2Obj.h"



class ExampleForCyclicDependency2Collection;
class ExampleForCyclicDependency2CollectionIterator;

/** @class ExampleForCyclicDependency2
 *  Type for cyclic dependency
 *  @author: Benedikt Hegner
 */
class ExampleForCyclicDependency2 {

  friend ExampleForCyclicDependency2Collection;
  friend ExampleForCyclicDependency2CollectionIterator;

public:

  /// default constructor
  ExampleForCyclicDependency2();

  /// constructor from existing ExampleForCyclicDependency2Obj
  ExampleForCyclicDependency2(ExampleForCyclicDependency2Obj* obj);
  /// copy constructor
  ExampleForCyclicDependency2(const ExampleForCyclicDependency2& other);
  /// copy-assignment operator
  ExampleForCyclicDependency2& operator=(const ExampleForCyclicDependency2& other);
  /// support cloning (deep-copy)
  ExampleForCyclicDependency2 clone() const;
  /// destructor
  ~ExampleForCyclicDependency2();

public:

  /// Access the  a ref
  const ::ExampleForCyclicDependency1 ref() const;

  /// Set the  a ref
  void ref(::ExampleForCyclicDependency1 value);



  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleForCyclicDependency2Obj instance
  void unlink(){m_obj = nullptr;}

  bool operator==(const ExampleForCyclicDependency2& other) const {
    return (m_obj==other.m_obj);
  }

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleForCyclicDependency2& p1,
//       const ExampleForCyclicDependency2& p2 );
  bool operator<(const ExampleForCyclicDependency2& other) const { return m_obj < other.m_obj  ; }

  const podio::ObjectID getObjectID() const;

private:
  ExampleForCyclicDependency2Obj* m_obj;

};



#endif
