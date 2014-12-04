#ifndef TrackHitHANDLE_H
#define TrackHitHANDLE_H
#include "datamodel/TrackHit.h"
#include "datamodel/BareHit.h"

#include <vector>

// contains basic hit information
// author: C. Bernet, B. Hegner

//forward declaration of TrackHit container
class TrackHitCollection;

namespace albers {
  class Registry;
}

class TrackHitHandle {

  friend class TrackHitCollection;

public:

  TrackHitHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  TrackHitHandle() = default;



  // precheck whether the pointee actually exists
  bool isAvailable() const;

  // returns a const (read-only) reference to the object pointed by the Handle.
  const TrackHit& read() const {return m_container->at(m_index);}

  // returns a non-const (writeable) reference to the object pointed by the Handle 
  TrackHit& mod() {return m_container->at(m_index);}
  
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

  /// equality operator (true if both the index and the container ID are equal)
  bool operator==(const TrackHitHandle& other) const {
       return (m_index==other.m_index) && (other.m_containerID==other.m_containerID);
  }

  /// less comparison operator, so that Handles can be e.g. stored in sets.
  friend bool operator< (const TrackHitHandle& p1,
			 const TrackHitHandle& p2 );

private:
  TrackHitHandle(int index, int containerID,  std::vector<TrackHit>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<TrackHit>* m_container; //! transient
  albers::Registry* m_registry; //! transient
  //  bool _retrieveData();
  // members to support 1-to-N relations
  

};

#endif
