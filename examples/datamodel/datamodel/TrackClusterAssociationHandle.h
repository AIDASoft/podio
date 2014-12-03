#ifndef TrackClusterAssociationHANDLE_H
#define TrackClusterAssociationHANDLE_H
#include "datamodel/TrackClusterAssociation.h"
#include "datamodel/TrackHandle.h"
#include "datamodel/TrackClusterHandle.h"

#include <vector>

// The cluster.
// author: C. Bernet, B. Hegner

//forward declaration of TrackClusterAssociation container
class TrackClusterAssociationCollection;

namespace albers {
  class Registry;
}

class TrackClusterAssociationHandle {

  friend class TrackClusterAssociationCollection;

public:

  TrackClusterAssociationHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  TrackClusterAssociationHandle() = default;



  // precheck whether the pointee actually exists
  bool isAvailable() const;

  // returns a const (read-only) reference to the object pointed by the Handle.
  const TrackClusterAssociation& read() const {return m_container->at(m_index);}

  // returns a non-const (writeable) reference to the object pointed by the Handle 
  TrackClusterAssociation& mod() {return m_container->at(m_index);}
  
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

  /// equality operator (true if both the index and the container ID are equal)
  bool operator==(const TrackClusterAssociationHandle& other) const {
       return (m_index==other.m_index) && (other.m_containerID==other.m_containerID);
  }

  /// less comparison operator, so that Handles can be e.g. stored in sets.
  friend bool operator< (const TrackClusterAssociationHandle& p1,
			 const TrackClusterAssociationHandle& p2 );

private:
  TrackClusterAssociationHandle(int index, int containerID,  std::vector<TrackClusterAssociation>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<TrackClusterAssociation>* m_container; //! transient
  albers::Registry* m_registry; //! transient
  //  bool _retrieveData();
  // members to support 1-to-N relations
  

};

#endif
