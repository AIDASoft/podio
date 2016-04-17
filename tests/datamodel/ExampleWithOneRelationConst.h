#ifndef ConstExampleWithOneRelation_H
#define ConstExampleWithOneRelation_H
#include "ExampleWithOneRelationData.h"
#include <vector>
#include "podio/ObjectID.h"

// Type with one relation member
// author: Benedikt Hegner

//forward declarations
class ExampleCluster;
class ConstExampleCluster;


#include "ExampleWithOneRelationObj.h"



class ExampleWithOneRelationObj;
class ExampleWithOneRelation;
class ExampleWithOneRelationCollection;
class ExampleWithOneRelationCollectionIterator;

class ConstExampleWithOneRelation {

  friend ExampleWithOneRelation;
  friend ExampleWithOneRelationCollection;
  friend ExampleWithOneRelationCollectionIterator;

public:

  /// default constructor
  ConstExampleWithOneRelation();
  
  /// constructor from existing ExampleWithOneRelationObj
  ConstExampleWithOneRelation(ExampleWithOneRelationObj* obj);
  /// copy constructor
  ConstExampleWithOneRelation(const ConstExampleWithOneRelation& other);
  /// copy-assignment operator
  ConstExampleWithOneRelation& operator=(const ConstExampleWithOneRelation& other);
  /// support cloning (deep-copy)
  ConstExampleWithOneRelation clone() const;
  /// destructor
  ~ConstExampleWithOneRelation();


public:

  const ::ConstExampleCluster cluster() const;



  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleWithOneRelationObj instance
  void unlink(){m_obj = nullptr;}

  bool operator==(const ConstExampleWithOneRelation& other) const {
       return (m_obj==other.m_obj);
  }

  bool operator==(const ExampleWithOneRelation& other) const;

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleWithOneRelation& p1,
//       const ExampleWithOneRelation& p2 );

  const podio::ObjectID getObjectID() const;

private:
  ExampleWithOneRelationObj* m_obj;

};


#endif
