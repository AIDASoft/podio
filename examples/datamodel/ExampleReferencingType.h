#ifndef ExampleReferencingType_H
#define ExampleReferencingType_H
#include "ExampleReferencingTypeData.h"
#include <vector>
#include "ExampleCluster.h"
#include "ExampleReferencingType.h"

#include <vector>
#include "albers/ObjectID.h"

// Referencing Type
// author: B. Hegner

//forward declarations
class ExampleReferencingTypeCollection;
class ExampleReferencingTypeCollectionIterator;
class ExampleReferencingTypeObj;

#include "ExampleReferencingTypeObj.h"

class ExampleReferencingType {

  friend ExampleReferencingTypeCollection;
  friend ExampleReferencingTypeCollectionIterator;

public:

  ExampleReferencingType();
  ExampleReferencingType(const ExampleReferencingType& other);
  ExampleReferencingType& operator=(const ExampleReferencingType& other);
  ExampleReferencingType(ExampleReferencingTypeObj* obj);
  ~ExampleReferencingType();



  void addClusters(ExampleCluster&);
  std::vector<ExampleCluster>::const_iterator Clusters_begin() const;
  std::vector<ExampleCluster>::const_iterator Clusters_end() const;
  void addRefs(ExampleReferencingType&);
  std::vector<ExampleReferencingType>::const_iterator Refs_begin() const;
  std::vector<ExampleReferencingType>::const_iterator Refs_end() const;

  bool isAvailable() const; // precheck whether the pointee actually exists
  void unlink(){m_obj = nullptr;};

  bool operator==(const ExampleReferencingType& other) const {
       return (m_obj==other.m_obj);
  }

  /// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleReferencingType& p1,
//       const ExampleReferencingType& p2 );

  const albers::ObjectID getObjectID() const;

private:
  ExampleReferencingTypeObj* m_obj;

};

#endif
