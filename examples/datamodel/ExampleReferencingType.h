#ifndef ExampleReferencingType_H
#define ExampleReferencingType_H
#include "ExampleReferencingTypeData.h"
#include <vector>
#include "ExampleCluster.h"

#include <vector>
#include "albers/ObjectID.h"
#include <iostream>

// Referencing Type
// author: B. Hegner

//forward declaration of ExampleReferencingType container
class ExampleReferencingTypeCollection;
class ExampleReferencingTypeCollectionIterator;

#include "ExampleReferencingTypeEntry.h"

namespace albers {
  class Registry;
}

class ExampleReferencingType {

  friend ExampleReferencingTypeCollection;
  friend ExampleReferencingTypeCollectionIterator;

public:

  ExampleReferencingType() : m_entry(new ExampleReferencingTypeEntry()){};
  ExampleReferencingType(const ExampleReferencingType& other) : m_entry(other.m_entry) {m_entry->increaseRefCount();};
  ExampleReferencingType& operator=(const ExampleReferencingType& other);
  ExampleReferencingType(ExampleReferencingTypeEntry* entry);
  ~ExampleReferencingType();



  void addClusters(ExampleCluster&);
  std::vector<ExampleCluster>::const_iterator Clusters_begin() const;
  std::vector<ExampleCluster>::const_iterator Clusters_end() const;

  bool isAvailable() const; // precheck whether the pointee actually exists
  void unlink(){m_entry = nullptr;};

  bool operator==(const ExampleReferencingType& other) const {
       return (m_entry==other.m_entry);
  }

  /// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleReferencingType& p1,
//       const ExampleReferencingType& p2 );

  const albers::ObjectID getObjectID() const;

private:
  ExampleReferencingTypeEntry* m_entry;

};

#endif
