#ifndef ExampleWithComponent_H
#define ExampleWithComponent_H
#include "ExampleWithComponentData.h"
#include "NotSoSimpleStruct.h"

#include <vector>
#include "podio/ObjectID.h"

//  a component
// author: Benedikt Hegner

//forward declarations
class ExampleWithComponentCollection;
class ExampleWithComponentCollectionIterator;
class ConstExampleWithComponent;


#include "ExampleWithComponentConst.h"
#include "ExampleWithComponentObj.h"

class ExampleWithComponent {

  friend ExampleWithComponentCollection;
  friend ExampleWithComponentCollectionIterator;
  friend ConstExampleWithComponent;

public:

  /// default constructor
  ExampleWithComponent();
    ExampleWithComponent(NotSoSimpleStruct component);

  /// constructor from existing ExampleWithComponentObj
  ExampleWithComponent(ExampleWithComponentObj* obj);
  /// copy constructor
  ExampleWithComponent(const ExampleWithComponent& other);
  /// copy-assignment operator
  ExampleWithComponent& operator=(const ExampleWithComponent& other);
  /// support cloning (deep-copy)
  ExampleWithComponent clone() const;
  /// destructor
  ~ExampleWithComponent();

  /// conversion to const object
  operator ConstExampleWithComponent () const;

public:

  const NotSoSimpleStruct& component() const { return m_obj->data.component; };

  NotSoSimpleStruct& component() { return m_obj->data.component; };
  void component(class NotSoSimpleStruct value);


  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleWithComponentObj instance
  void unlink(){m_obj = nullptr;};

  bool operator==(const ExampleWithComponent& other) const {
       return (m_obj==other.m_obj);
  }

  bool operator==(const ConstExampleWithComponent& other) const;

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleWithComponent& p1,
//       const ExampleWithComponent& p2 );

  const podio::ObjectID getObjectID() const;

private:
  ExampleWithComponentObj* m_obj;

};

#endif
