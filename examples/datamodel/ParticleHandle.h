#ifndef ParticleHANDLE_H
#define ParticleHANDLE_H
#include "Particle.h"
#include "LorentzVector.h"

#include <vector>
//forward declaration of Particle container
class ParticleCollection;

namespace albers {
  class Registry;
}

class ParticleHandle {

  friend ParticleCollection;

public:

 ParticleHandle(){};

  // COLIN: too painful to call each setter one by one, and unsafe. remove setters and use a parameter list in the constructor? or an init function2222
  const int& ID() const;
  const LorentzVector& P4() const;

  void setID(int value);
  void setP4(LorentzVector value);


  // COLIN: I'd make that a true const method, and would set m_container in prepareAFterRead. What if the user doesn't call that?
  bool isAvailable() const; // precheck whether the pointee actually exists
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

private:
  ParticleHandle(int index, int containerID,  std::vector<Particle>* container);
  int m_index;
  int m_containerID;
  // COLIN: after reading, the transient m_container address must be taken from the registry using the persistent m_containerID and set. This seems to happen in isAvailable... why not in prepareAfterRead?
  mutable std::vector<Particle>* m_container; //! transient
  albers::Registry* m_registry; //! transient
//  bool _retrieveData();

};

#endif
