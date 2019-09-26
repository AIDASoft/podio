#ifndef ConstExampleWithComponent_H
#define ConstExampleWithComponent_H
#include "NotSoSimpleStruct.h"
#include "ExampleWithComponentData.h"
#include <vector>
#include "podio/ObjectID.h"

//forward declarations


#include "ExampleWithComponentObj.h"



class ExampleWithComponentObj;
class ExampleWithComponent;
class ExampleWithComponentCollection;
class ExampleWithComponentCollectionIterator;

/** @class ConstExampleWithComponent
 *  Type with one component
 *  @author: Benedikt Hegner
 */

class ConstExampleWithComponent {

  friend ExampleWithComponent;
  friend ExampleWithComponentCollection;
  friend ExampleWithComponentCollectionIterator;

public:

  /// default constructor
  ConstExampleWithComponent();
  ConstExampleWithComponent(NotSoSimpleStruct component);

  /// constructor from existing ExampleWithComponentObj
  ConstExampleWithComponent(ExampleWithComponentObj* obj);
  /// copy constructor
  ConstExampleWithComponent(const ConstExampleWithComponent& other);
  /// copy-assignment operator
  ConstExampleWithComponent& operator=(const ConstExampleWithComponent& other);
  /// support cloning (deep-copy)
  ConstExampleWithComponent clone() const;
  /// destructor
  ~ConstExampleWithComponent();


public:

  /// Access the  a component
  const NotSoSimpleStruct& component() const;
  /// Access the member of  a component
  const SimpleStruct& data() const;



  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleWithComponentObj instance
  void unlink(){m_obj = nullptr;}

  bool operator==(const ConstExampleWithComponent& other) const {
       return (m_obj==other.m_obj);
  }

  bool operator==(const ExampleWithComponent& other) const;

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleWithComponent& p1,
//       const ExampleWithComponent& p2 );
  bool operator<(const ConstExampleWithComponent& other) const { return m_obj < other.m_obj  ; }

  unsigned int id() const { return getObjectID().collectionID * 10000000 + getObjectID().index  ;  } 

  const podio::ObjectID getObjectID() const;

private:
  ExampleWithComponentObj* m_obj;

};


#endif
