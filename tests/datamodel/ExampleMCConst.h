#ifndef ConstExampleMC_H
#define ConstExampleMC_H
#include "ExampleMC.h"
#include "ExampleMCData.h"
#include "podio/ObjectID.h"
#include <vector>

// forward declarations

#include "ExampleMCObj.h"

class ExampleMCObj;
class ExampleMC;
class ExampleMCCollection;
class ExampleMCCollectionIterator;

/** @class ConstExampleMC
 *  Example MC-particle
 *  @author: F.Gaede
 */

class ConstExampleMC {

  friend ExampleMC;
  friend ExampleMCCollection;
  friend ExampleMCCollectionIterator;

public:
  /// default constructor
  ConstExampleMC();
  ConstExampleMC(double energy, int PDG);

  /// constructor from existing ExampleMCObj
  ConstExampleMC(ExampleMCObj *obj);
  /// copy constructor
  ConstExampleMC(const ConstExampleMC &other);
  /// copy-assignment operator
  ConstExampleMC &operator=(const ConstExampleMC &other);
  /// support cloning (deep-copy)
  ConstExampleMC clone() const;
  /// destructor
  ~ConstExampleMC();

public:
  /// Access the  energy
  const double &energy() const;
  /// Access the  PDG code
  const int &PDG() const;

  unsigned int parents_size() const;
  ::ConstExampleMC parents(unsigned int) const;
  std::vector<::ConstExampleMC>::const_iterator parents_begin() const;
  std::vector<::ConstExampleMC>::const_iterator parents_end() const;
  unsigned int daughters_size() const;
  ::ConstExampleMC daughters(unsigned int) const;
  std::vector<::ConstExampleMC>::const_iterator daughters_begin() const;
  std::vector<::ConstExampleMC>::const_iterator daughters_end() const;

  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleMCObj instance
  void unlink() { m_obj = nullptr; }

  bool operator==(const ConstExampleMC &other) const {
    return (m_obj == other.m_obj);
  }

  bool operator==(const ExampleMC &other) const;

  // less comparison operator, so that objects can be e.g. stored in sets.
  //  friend bool operator< (const ExampleMC& p1,
  //       const ExampleMC& p2 );
  bool operator<(const ConstExampleMC &other) const {
    return m_obj < other.m_obj;
  }

  unsigned int id() const {
    return getObjectID().collectionID * 10000000 + getObjectID().index;
  }

  const podio::ObjectID getObjectID() const;

private:
  ExampleMCObj *m_obj;
};

#endif
