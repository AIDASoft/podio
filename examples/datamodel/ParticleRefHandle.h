#ifndef ParticleRefHANDLE_H
#define ParticleRefHANDLE_H
#include "ParticleRef.h"
#include "ParticleHandle.h"
#include <vector>
//forward declaration of ParticleRef container
class ParticleRefCollection;

namespace albers {
  class Registry;
}

class ParticleRefHandle {

  friend ParticleRefCollection;

public:

 ParticleRefHandle(){};

  const ParticleHandle& Particle() const;

  void setParticle(ParticleHandle value);


  bool isAvailable() const; // precheck whether the pointee actually exists
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

private:
  
  ParticleRefHandle(int index, int containerID,  std::vector<ParticleRef>* container);
  int m_index; 
  int m_containerID;
  mutable std::vector<ParticleRef>* m_container; //! transient
  albers::Registry* m_registry; //! transient
//  bool _retrieveData();
  
};

#endif
