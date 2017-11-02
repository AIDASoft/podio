#ifndef ExampleWithComponent_H
#define ExampleWithComponent_H
#include "ExampleWithComponentData.h"
#include "NotSoSimpleStruct.h"
#include "podio/ObjectID.h"
#include <iomanip>
#include <iostream>
#include <vector>

// forward declarations

#include "ExampleWithComponentConst.h"
#include "ExampleWithComponentObj.h"

class ExampleWithComponentCollection;
class ExampleWithComponentCollectionIterator;
class ConstExampleWithComponent;

/** @class ExampleWithComponent
 *  Type with one component
 *  @author: Benedikt Hegner
 */
class ExampleWithComponent {

  friend ExampleWithComponentCollection;
  friend ExampleWithComponentCollectionIterator;
  friend ConstExampleWithComponent;

public:
  /// default constructor
  ExampleWithComponent();
  ExampleWithComponent(NotSoSimpleStruct component);

  /// constructor from existing ExampleWithComponentObj
  ExampleWithComponent(ExampleWithComponentObj *obj);
  /// copy constructor
  ExampleWithComponent(const ExampleWithComponent &other);
  /// copy-assignment operator
  ExampleWithComponent &operator=(const ExampleWithComponent &other);
  /// support cloning (deep-copy)
  ExampleWithComponent clone() const;
  /// destructor
  ~ExampleWithComponent();

  /// conversion to const object
  operator ConstExampleWithComponent() const;

public:
  /// Access the  a component
  const NotSoSimpleStruct &component() const;
  /// Access the member of  a component
  const SimpleStruct &data() const;

  /// Get reference to the  a component
  NotSoSimpleStruct &component();
  /// Set the  a component
  void component(class NotSoSimpleStruct value);
  /// Get reference to the member of  a component
  SimpleStruct &data();
  /// Set the  member of  a component
  void data(class SimpleStruct value);

  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleWithComponentObj instance
  void unlink() { m_obj = nullptr; }

  bool operator==(const ExampleWithComponent &other) const {
    return (m_obj == other.m_obj);
  }

  bool operator==(const ConstExampleWithComponent &other) const;

  // less comparison operator, so that objects can be e.g. stored in sets.
  //  friend bool operator< (const ExampleWithComponent& p1,
  //       const ExampleWithComponent& p2 );
  bool operator<(const ExampleWithComponent &other) const {
    return m_obj < other.m_obj;
  }

  unsigned int id() const {
    return getObjectID().collectionID * 10000000 + getObjectID().index;
  }

  const podio::ObjectID getObjectID() const;

private:
  ExampleWithComponentObj *m_obj;
};

std::ostream &operator<<(std::ostream &o,
                         const ConstExampleWithComponent &value);

#endif
