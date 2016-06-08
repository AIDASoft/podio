#ifndef ExampleMC_H
#define ExampleMC_H
#include "ExampleMCData.h"
#include <vector>
#include "ExampleMC.h"
#include "ExampleMC.h"
#include <vector>
#include "podio/ObjectID.h"

// Example MC-particle
// author: F.Gaede

//forward declarations


#include "ExampleMCObj.h"



class ExampleMCCollection;
class ExampleMCCollectionIterator;

class ExampleMC {

  friend ExampleMCCollection;
  friend ExampleMCCollectionIterator;

public:

  /// default constructor
  ExampleMC();
  ExampleMC(double energy,int PDG);

  /// constructor from existing ExampleMCObj
  ExampleMC(ExampleMCObj* obj);
  /// copy constructor
  ExampleMC(const ExampleMC& other);
  /// copy-assignment operator
  ExampleMC& operator=(const ExampleMC& other);
  /// support cloning (deep-copy)
  ExampleMC clone() const;
  /// destructor
  ~ExampleMC();

public:

  const double& energy() const;
  const int& PDG() const;

  void energy(double value);

  void PDG(int value);


  void addparents(ExampleMC);
  unsigned int parents_size() const;
  ExampleMC parents(unsigned int) const;
  std::vector<ExampleMC>::const_iterator parents_begin() const;
  std::vector<ExampleMC>::const_iterator parents_end() const;

  void adddaughters(ExampleMC);
  unsigned int daughters_size() const;
  ExampleMC daughters(unsigned int) const;
  std::vector<ExampleMC>::const_iterator daughters_begin() const;
  std::vector<ExampleMC>::const_iterator daughters_end() const;



  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleMCObj instance
  void unlink(){m_obj = nullptr;}

  bool operator==(const ExampleMC& other) const {
    return (m_obj==other.m_obj);
  }

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleMC& p1,
//       const ExampleMC& p2 );
  bool operator<(const ExampleMC& other) const { return m_obj < other.m_obj  ; }

  const podio::ObjectID getObjectID() const;

private:
  ExampleMCObj* m_obj;

};



#endif
