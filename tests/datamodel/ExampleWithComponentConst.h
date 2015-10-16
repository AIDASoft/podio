#ifndef ConstExampleWithComponent_H
#define ConstExampleWithComponent_H
#include "ExampleWithComponentData.h"
#include "NotSoSimpleStruct.h"

#include <vector>
#include "podio/ObjectID.h"

//  a component
// author: Bendikt Hegner

//forward declarations
class ExampleWithComponentCollection;
class ExampleWithComponentCollectionIterator;


#include "ExampleWithComponentObj.h"

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

  const NotSoSimpleStruct& component() const { return m_obj->data.component; };


  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleWithComponentObj instance
  void unlink(){m_obj = nullptr;};

  bool operator==(const ConstExampleWithComponent& other) const {
       return (m_obj==other.m_obj);
  }

  bool operator==(const ExampleWithComponent& other) const;

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleWithComponent& p1,
//       const ExampleWithComponent& p2 );

  const podio::ObjectID getObjectID() const;

private:
  ExampleWithComponentObj* m_obj;

};

#endif
