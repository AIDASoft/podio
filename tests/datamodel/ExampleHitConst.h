#ifndef ConstExampleHit_H
#define ConstExampleHit_H
#include "ExampleHitData.h"

#include <vector>
#include "podio/ObjectID.h"

//  measured energy deposit
// author: B. Hegner

//forward declarations
class ExampleHitCollection;
class ExampleHitCollectionIterator;


#include "ExampleHitObj.h"

class ConstExampleHit {

  friend ExampleHit;
  friend ExampleHitCollection;
  friend ExampleHitCollectionIterator;

public:

  /// default constructor
  ConstExampleHit();
  ConstExampleHit(double x,double y,double z,double energy);

  /// constructor from existing ExampleHitObj
  ConstExampleHit(ExampleHitObj* obj);
  /// copy constructor
  ConstExampleHit(const ConstExampleHit& other);
  /// copy-assignment operator
  ConstExampleHit& operator=(const ConstExampleHit& other);
  /// support cloning (deep-copy)
  ConstExampleHit clone() const;
  /// destructor
  ~ConstExampleHit();


public:

  const double& x() const { return m_obj->data.x; };
  const double& y() const { return m_obj->data.y; };
  const double& z() const { return m_obj->data.z; };
  const double& energy() const { return m_obj->data.energy; };


  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleHitObj instance
  void unlink(){m_obj = nullptr;};

  bool operator==(const ConstExampleHit& other) const {
       return (m_obj==other.m_obj);
  }

  bool operator==(const ExampleHit& other) const;

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleHit& p1,
//       const ExampleHit& p2 );

  const podio::ObjectID getObjectID() const;

private:
  ExampleHitObj* m_obj;

};

#endif
