#ifndef TrackClusterHANDLE_H
#define TrackClusterHANDLE_H
#include "datamodel/TrackCluster.h"
#include "datamodel/BareCluster.h"

#include <vector>

// contains basic cluster information
// author: C. Bernet, B. Hegner

//forward declaration of TrackCluster container
class TrackClusterCollection;

namespace albers {
  class Registry;
}

class TrackClusterHandle {

  friend class TrackClusterCollection;

public:

  TrackClusterHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  TrackClusterHandle() = default;



  // precheck whether the pointee actually exists
  bool isAvailable() const;

  // returns a const (read-only) reference to the object pointed by the Handle.
  const TrackCluster& read() const {return m_container->at(m_index);}

  // returns a non-const (writeable) reference to the object pointed by the Handle 
  TrackCluster& mod() {return m_container->at(m_index);}
  
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

  /// equality operator (true if both the index and the container ID are equal)
  bool operator==(const TrackClusterHandle& other) const {
       return (m_index==other.m_index) && (other.m_containerID==other.m_containerID);
  }

  /// less comparison operator, so that Handles can be e.g. stored in sets.
  friend bool operator< (const TrackClusterHandle& p1,
			 const TrackClusterHandle& p2 );

private:
  TrackClusterHandle(int index, int containerID,  std::vector<TrackCluster>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<TrackCluster>* m_container; //! transient
  albers::Registry* m_registry; //! transient
  //  bool _retrieveData();
  // members to support 1-to-N relations
  

};

#endif
