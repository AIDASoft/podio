#ifndef ExampleHit_H
#define ExampleHit_H
#include "ExampleHitData.h"
#include <vector>
#include "podio/ObjectID.h"

//forward declarations


#include "ExampleHitObj.h"



class ExampleHitCollection;
class ExampleHitCollectionIterator;

/** @class ExampleHit
 *  Example Hit
 *  @author: B. Hegner
 */
class ExampleHit {

  friend ExampleHitCollection;
  friend ExampleHitCollectionIterator;

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

public:

  /// Access the  x-coordinate
  const double& x() const;
  /// Access the  y-coordinate
  const double& y() const;
  /// Access the  z-coordinate
  const double& z() const;
  /// Access the  measured energy deposit
  const double& energy() const;

  /// Set the  x-coordinate
  void x(double value);

  /// Set the  y-coordinate
  void y(double value);

  /// Set the  z-coordinate
  void z(double value);

  /// Set the  measured energy deposit
  void energy(double value);




  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleHitObj instance
  void unlink(){m_obj = nullptr;}

  bool operator==(const ExampleHit& other) const {
    return (m_obj==other.m_obj);
  }

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleHit& p1,
//       const ExampleHit& p2 );
  bool operator<(const ExampleHit& other) const { return m_obj < other.m_obj  ; }

  const podio::ObjectID getObjectID() const;

private:
  ExampleHitObj* m_obj;

};



#endif
