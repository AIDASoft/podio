#ifndef MCParticleAssociationHANDLE_H
#define MCParticleAssociationHANDLE_H
#include "datamodel/MCParticleAssociation.h"
#include "datamodel/MCParticleHandle.h"
#include "datamodel/MCParticleHandle.h"

#include <vector>

// The daughter.
// author: C. Bernet, B. Hegner

//forward declaration of MCParticleAssociation container
class MCParticleAssociationCollection;

namespace albers {
  class Registry;
}

class MCParticleAssociationHandle {

  friend class MCParticleAssociationCollection;

public:

  MCParticleAssociationHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  MCParticleAssociationHandle() = default;



  // precheck whether the pointee actually exists
  bool isAvailable() const;

  // returns a const (read-only) reference to the object pointed by the Handle.
  const MCParticleAssociation& read() const {return m_container->at(m_index);}

  // returns a non-const (writeable) reference to the object pointed by the Handle 
  MCParticleAssociation& mod() {return m_container->at(m_index);}
  
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

  /// equality operator (true if both the index and the container ID are equal)
  bool operator==(const MCParticleAssociationHandle& other) const {
       return (m_index==other.m_index) && (other.m_containerID==other.m_containerID);
  }

  /// less comparison operator, so that Handles can be e.g. stored in sets.
  friend bool operator< (const MCParticleAssociationHandle& p1,
			 const MCParticleAssociationHandle& p2 );

private:
  MCParticleAssociationHandle(int index, int containerID,  std::vector<MCParticleAssociation>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<MCParticleAssociation>* m_container; //! transient
  albers::Registry* m_registry; //! transient
  //  bool _retrieveData();
  // members to support 1-to-N relations
  

};

#endif
