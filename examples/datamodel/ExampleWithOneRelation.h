#ifndef ExampleWithOneRelation_H
#define ExampleWithOneRelation_H
#include "ExampleWithOneRelationData.h"

#include <vector>
#include "podio/ObjectID.h"

// Type with one relation member
// author: Benedikt Hegner

//forward declarations
class ExampleCluster;
class ConstExampleCluster;


#include "ExampleWithOneRelationConst.h"
#include "ExampleWithOneRelationObj.h"



class ExampleWithOneRelationCollection;
class ExampleWithOneRelationCollectionIterator;
class ConstExampleWithOneRelation;

class ExampleWithOneRelation {

  friend ExampleWithOneRelationCollection;
  friend ExampleWithOneRelationCollectionIterator;
  friend ConstExampleWithOneRelation;

public:

  /// default constructor
  ExampleWithOneRelation();
  
  /// constructor from existing ExampleWithOneRelationObj
  ExampleWithOneRelation(ExampleWithOneRelationObj* obj);
  /// copy constructor
  ExampleWithOneRelation(const ExampleWithOneRelation& other);
  /// copy-assignment operator
  ExampleWithOneRelation& operator=(const ExampleWithOneRelation& other);
  /// support cloning (deep-copy)
  ExampleWithOneRelation clone() const;
  /// destructor
  ~ExampleWithOneRelation();

  /// conversion to const object
  operator ConstExampleWithOneRelation () const;

public:

  const ConstExampleCluster cluster() const;

  void cluster(ConstExampleCluster value);


  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleWithOneRelationObj instance
  void unlink(){m_obj = nullptr;}

  bool operator==(const ExampleWithOneRelation& other) const {
       return (m_obj==other.m_obj);
  }

  bool operator==(const ConstExampleWithOneRelation& other) const;

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleWithOneRelation& p1,
//       const ExampleWithOneRelation& p2 );

  const podio::ObjectID getObjectID() const;

private:
  ExampleWithOneRelationObj* m_obj;

};



#endif
