#ifndef JetParticleHANDLE_H
#define JetParticleHANDLE_H
#include "JetParticle.h"
#include "JetHandle.h"
#include "ParticleHandle.h"
#include <vector>
//forward declaration of JetParticle container
class JetParticleCollection;

namespace albers {
  class Registry;
}

class JetParticleHandle {

  friend JetParticleCollection;

public:

 JetParticleHandle(){};

  // COLIN: too painful to call each setter one by one, and unsafe. remove setters and use a parameter list in the constructor? or an init function2222
  const JetHandle& Jet() const;
  const ParticleHandle& Particle() const;

  void setJet(JetHandle value);
  void setParticle(ParticleHandle value);


  // COLIN: I'd make that a true const method, and would set m_container in prepareAFterRead. What if the user doesn't call that?
  bool isAvailable() const; // precheck whether the pointee actually exists
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

private:
  JetParticleHandle(int index, int containerID,  std::vector<JetParticle>* container);
  int m_index;
  int m_containerID;
  // COLIN: after reading, the transient m_container address must be taken from the registry using the persistent m_containerID and set. This seems to happen in isAvailable... why not in prepareAfterRead?
  mutable std::vector<JetParticle>* m_container; //! transient
  albers::Registry* m_registry; //! transient
//  bool _retrieveData();

};

#endif
