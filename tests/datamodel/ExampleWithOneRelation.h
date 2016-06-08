#ifndef ExampleWithOneRelation_H
#define ExampleWithOneRelation_H
#include "ExampleWithOneRelationData.h"
#include <vector>
#include "podio/ObjectID.h"

// Type with one relation member
// author: Benedikt Hegner

//forward declarations
class ExampleCluster;


#include "ExampleWithOneRelationObj.h"



class ExampleWithOneRelationCollection;
class ExampleWithOneRelationCollectionIterator;

class ExampleWithOneRelation {

  friend ExampleWithOneRelationCollection;
  friend ExampleWithOneRelationCollectionIterator;

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

public:

  const ::ExampleCluster cluster() const;

  void cluster(::ExampleCluster value);



  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleWithOneRelationObj instance
  void unlink(){m_obj = nullptr;}

  bool operator==(const ExampleWithOneRelation& other) const {
    return (m_obj==other.m_obj);
  }

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleWithOneRelation& p1,
//       const ExampleWithOneRelation& p2 );
  bool operator<(const ExampleWithOneRelation& other) const { return m_obj < other.m_obj  ; }

  const podio::ObjectID getObjectID() const;

private:
  ExampleWithOneRelationObj* m_obj;

};



#endif
