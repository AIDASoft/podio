#ifndef ExampleReferencingType_H
#define ExampleReferencingType_H
#include "ExampleReferencingTypeData.h"
#include <vector>
#include "ExampleCluster.h"
#include "ExampleReferencingType.h"
#include <vector>
#include "podio/ObjectID.h"

//forward declarations


#include "ExampleReferencingTypeObj.h"



class ExampleReferencingTypeCollection;
class ExampleReferencingTypeCollectionIterator;

/** @class ExampleReferencingType
 *  Referencing Type
 *  @author: B. Hegner
 */
class ExampleReferencingType {

  friend ExampleReferencingTypeCollection;
  friend ExampleReferencingTypeCollectionIterator;

public:

  /// default constructor
  ExampleReferencingType();

  /// constructor from existing ExampleReferencingTypeObj
  ExampleReferencingType(ExampleReferencingTypeObj* obj);
  /// copy constructor
  ExampleReferencingType(const ExampleReferencingType& other);
  /// copy-assignment operator
  ExampleReferencingType& operator=(const ExampleReferencingType& other);
  /// support cloning (deep-copy)
  ExampleReferencingType clone() const;
  /// destructor
  ~ExampleReferencingType();

public:



  void addClusters(::ExampleCluster);
  unsigned int Clusters_size() const;
  ::ExampleCluster Clusters(unsigned int) const;
  std::vector<::ExampleCluster>::const_iterator Clusters_begin() const;
  std::vector<::ExampleCluster>::const_iterator Clusters_end() const;

  void addRefs(::ExampleReferencingType);
  unsigned int Refs_size() const;
  ::ExampleReferencingType Refs(unsigned int) const;
  std::vector<::ExampleReferencingType>::const_iterator Refs_begin() const;
  std::vector<::ExampleReferencingType>::const_iterator Refs_end() const;



  /// check whether the object is actually available
  bool isAvailable() const;
  /// disconnect from ExampleReferencingTypeObj instance
  void unlink(){m_obj = nullptr;}

  bool operator==(const ExampleReferencingType& other) const {
    return (m_obj==other.m_obj);
  }

// less comparison operator, so that objects can be e.g. stored in sets.
//  friend bool operator< (const ExampleReferencingType& p1,
//       const ExampleReferencingType& p2 );
  bool operator<(const ExampleReferencingType& other) const { return m_obj < other.m_obj  ; }

  const podio::ObjectID getObjectID() const;

private:
  ExampleReferencingTypeObj* m_obj;

};



#endif
