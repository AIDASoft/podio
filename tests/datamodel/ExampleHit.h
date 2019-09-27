#ifndef ExampleHit_H
#define ExampleHit_H
#include "ExampleHitData.h"
#include <vector>
#include <iostream>
#include <iomanip>
#include "podio/ObjectID.h"

//forward declarations


#include "ExampleHitConst.h"
#include "ExampleHitObj.h"



class ExampleHitCollection;
class ExampleHitCollectionIterator;
class ConstExampleHit;

/** @class ExampleHit
 *  Example Hit
 *  @author: B. Hegner
 */
class ExampleHit {

  friend ExampleHitCollection;
  friend ExampleHitCollectionIterator;
  friend ConstExampleHit;

public:

  /// default constructor
  ExampleHit();
  ExampleHit(unsigned long long cellID,double x,double y,double z,double energy);

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

  /// Access the  cellID
  const unsigned long long& cellID() const;
  /// Access the  x-coordinate
  const double& x() const;
  /// Access the  y-coordinate
  const double& y() const;
  /// Access the  z-coordinate
  const double& z() const;
  /// Access the  measured energy deposit
  const double& energy() const;

  /// Set the  cellID
  void cellID(unsigned long long value);

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

  bool operator==(const ConstExampleHit& other) const;

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleHit& p1,
//       const ExampleHit& p2 );
  bool operator<(const ExampleHit& other) const { return m_obj < other.m_obj  ; }


  unsigned int id() const { return getObjectID().collectionID * 10000000 + getObjectID().index  ;  } 

  const podio::ObjectID getObjectID() const;

private:
  ExampleHitObj* m_obj;

};

std::ostream& operator<<( std::ostream& o,const ConstExampleHit& value );




#endif
