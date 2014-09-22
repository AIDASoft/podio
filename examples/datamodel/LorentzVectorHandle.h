#ifndef LorentzVectorHANDLE_H
#define LorentzVectorHANDLE_H
#include "LorentzVector.h"

#include <vector>
//forward declaration of LorentzVector container
class LorentzVectorCollection;

namespace albers {
  class Registry;
}

class LorentzVectorHandle {

  friend LorentzVectorCollection;

public:

 LorentzVectorHandle(){};

  // COLIN: too painful to call each setter one by one, and unsafe. remove setters and use a parameter list in the constructor? or an init function2222
  const float& Phi() const;
  const float& Eta() const;
  const float& Mass() const;
  const float& Pt() const;

  void setPhi(float value);
  void setEta(float value);
  void setMass(float value);
  void setPt(float value);


  // COLIN: I'd make that a true const method, and would set m_container in prepareAFterRead. What if the user doesn't call that?
  bool isAvailable() const; // precheck whether the pointee actually exists
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

private:
  LorentzVectorHandle(int index, int containerID,  std::vector<LorentzVector>* container);
  int m_index;
  int m_containerID;
  // COLIN: after reading, the transient m_container address must be taken from the registry using the persistent m_containerID and set. This seems to happen in isAvailable... why not in prepareAfterRead?
  mutable std::vector<LorentzVector>* m_container; //! transient
  albers::Registry* m_registry; //! transient
//  bool _retrieveData();

};

#endif
