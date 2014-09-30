#ifndef ParticleAssociationHANDLE_H
#define ParticleAssociationHANDLE_H
#include "ParticleAssociation.h"
#include "ParticleHandle.h"
#include "ParticleHandle.h"

#include <vector>

// Reference to the second particle
// author: C. Bernet, B. Hegner

//forward declaration of ParticleAssociation container
class ParticleAssociationCollection;

namespace albers {
  class Registry;
}

class ParticleAssociationHandle {

  friend ParticleAssociationCollection;

public:

ParticleAssociationHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  ParticleAssociationHandle() = default;

  // COLIN: too painful to call each setter one by one, and unsafe. remove setters and use a parameter list in the constructor? or an init function2222
  const ParticleHandle& First() const;
  const ParticleHandle& Second() const;

  void setFirst(ParticleHandle value);
  void setSecond(ParticleHandle value);


  // COLIN: I'd make that a true const method, and would set m_container in prepareAFterRead. What if the user doesn't call that?
  bool isAvailable() const; // precheck whether the pointee actually exists
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

  /// equality operator (true if both the index and the container ID are equal)
  bool operator==(const ParticleAssociationHandle& other) const {
       return (m_index==other.m_index) && (other.m_containerID==other.m_containerID);
  }

  /// less comparison operator, so that Handles can be e.g. stored in sets.
  friend bool operator< (const ParticleAssociationHandle& p1,
			 const ParticleAssociationHandle& p2 );

private:
  ParticleAssociationHandle(int index, int containerID,  std::vector<ParticleAssociation>* container);
  int m_index;
  int m_containerID;
  // COLIN: after reading, the transient m_container address must be taken from the registry using the persistent m_containerID and set. This seems to happen in isAvailable... why not in prepareAfterRead?
  mutable std::vector<ParticleAssociation>* m_container; //! transient
  albers::Registry* m_registry; //! transient
//  bool _retrieveData();

};

#endif
