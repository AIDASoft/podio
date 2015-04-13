#ifndef ExampleHit_H
#define ExampleHit_H
#include "ExampleHitData.h"

#include <vector>
#include "albers/ObjectID.h"
#include <iostream>

// measured energy deposit
// author: B. Hegner

//forward declaration of ExampleHit container
class ExampleHitCollection;
class ExampleHitCollectionIterator;

#include "ExampleHitEntry.h"

namespace albers {
  class Registry;
}

class ExampleHit {

  friend ExampleHitCollection;
  friend ExampleHitCollectionIterator;

public:

  ExampleHit() : m_entry(new ExampleHitEntry()){};
  ExampleHit(const ExampleHit& other) : m_entry(other.m_entry) {m_entry->increaseRefCount();};
  ExampleHit& operator=(const ExampleHit& other);
  ExampleHit(ExampleHitEntry* entry);
  ~ExampleHit();

  const double& x() const;
  const double& y() const;
  const double& z() const;
  const double& energy() const;

  void x(double value);
  void y(double value);
  void z(double value);
  void energy(double value);


  bool isAvailable() const; // precheck whether the pointee actually exists
  void unlink(){m_entry = nullptr;};

  bool operator==(const ExampleHit& other) const {
       return (m_entry==other.m_entry);
  }

  /// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleHit& p1,
//       const ExampleHit& p2 );

  const albers::ObjectID getObjectID() const;

private:
  ExampleHitEntry* m_entry;

};

#endif
