#ifndef ParticleHANDLE_H
#define ParticleHANDLE_H
#include "Particle.h"

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

  const float& Pz() const;
  const float& Px() const;
  const float& Py() const;
  const float& Energy() const;
  const int& Charge() const;

  void setPz(float value);
  void setPx(float value);
  void setPy(float value);
  void setEnergy(float value);
  void setCharge(int value);


  bool isAvailable() const; // precheck whether the pointee actually exists
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

private:
  
  ParticleHandle(int index, int containerID,  std::vector<Particle>* container);
  int m_index; 
  int m_containerID;
  mutable std::vector<Particle>* m_container; //! transient
  albers::Registry* m_registry; //! transient
//  bool _retrieveData();
  
};

#endif
