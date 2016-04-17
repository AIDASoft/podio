#ifndef ExampleHit_H
#define ExampleHit_H
#include "ExampleHitData.h"
#include <vector>
#include "podio/ObjectID.h"

// Example Hit
// author: B. Hegner

//forward declarations


#include "ExampleHitConst.h"
#include "ExampleHitObj.h"



class ExampleHitCollection;
class ExampleHitCollectionIterator;
class ConstExampleHit;

class ExampleHit {

  friend ExampleHitCollection;
  friend ExampleHitCollectionIterator;
  friend ConstExampleHit;

public:

  /// default constructor
  ExampleHit();
  ExampleHit(double x,double y,double z,double energy);

  /// constructor from existing ExampleHitObj
  ExampleHit(ExampleHitObj* obj);
  /// copy constructor
  ExampleHit(const ExampleHit& other);
  /// copy-assignment operator
  ExampleHit& operator=(const ExampleHit& other);
  /// support cloning (deep-copy)
  ExampleHit clone() const;
  /// destructor
  ~ExampleHit();

  /// conversion to const object
  operator ConstExampleHit () const;

public:

  const double& x() const;
  const double& y() const;
  const double& z() const;
  const double& energy() const;

  void x(double value);

  void y(double value);

  void z(double value);

  void energy(double value);




  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleHitObj instance
  void unlink(){m_obj = nullptr;}

  bool operator==(const ExampleHit& other) const {
    return (m_obj==other.m_obj);
  }

  bool operator==(const ConstExampleHit& other) const;

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleHit& p1,
//       const ExampleHit& p2 );

  const podio::ObjectID getObjectID() const;

private:
  ExampleHitObj* m_obj;

};



#endif
