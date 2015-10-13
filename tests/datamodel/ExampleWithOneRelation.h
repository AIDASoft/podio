#ifndef ExampleWithOneRelation_H
#define ExampleWithOneRelation_H
#include "ExampleWithOneRelationData.h"
#include "ExampleCluster.h"

#include <vector>
#include "podio/ObjectID.h"

// Type with one relation member
// author: Benedikt Hegner

//forward declarations
class ExampleWithOneRelationCollection;
class ExampleWithOneRelationCollectionIterator;
class ExampleWithOneRelationObj;

#include "ExampleWithOneRelationObj.h"

class ExampleWithOneRelation {

  friend ExampleWithOneRelationCollection;
  friend ExampleWithOneRelationCollectionIterator;

public:

  /// default constructor
  ExampleWithOneRelation();
  
  /// copy constructor
  ExampleWithOneRelation(const ExampleWithOneRelation& other);
  /// copy-assignment operator
  ExampleWithOneRelation& operator=(const ExampleWithOneRelation& other);
  /// constructor from existing ExampleWithOneRelationObj
  ExampleWithOneRelation(ExampleWithOneRelationObj* obj);
  /// support cloning (deep-copy)
  ExampleWithOneRelation clone() const;
  /// destructor
  ~ExampleWithOneRelation();

  const ExampleCluster& cluster() const { return m_obj->m_cluster; };

  void cluster(ExampleCluster value) { m_obj->m_cluster = value; };


  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleWithOneRelationObj instance
  void unlink(){m_obj = nullptr;};

  bool operator==(const ExampleWithOneRelation& other) const {
       return (m_obj==other.m_obj);
  }

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleWithOneRelation& p1,
//       const ExampleWithOneRelation& p2 );

  const podio::ObjectID getObjectID() const;

private:
  ExampleWithOneRelationObj* m_obj;

};

#endif
