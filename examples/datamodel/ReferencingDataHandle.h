#ifndef ReferencingDataHANDLE_H
#define ReferencingDataHANDLE_H
#include "ReferencingData.h"
#include "DummyDataHandle.h"

#include <vector>

// ref to dummy data
// author: C. Bernet, B. Hegner

//forward declaration of ReferencingData container
class ReferencingDataCollection;

namespace albers {
  class Registry;
}

class ReferencingDataHandle {

  friend ReferencingDataCollection;

public:

ReferencingDataHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  ReferencingDataHandle() = default;

  // COLIN: too painful to call each setter one by one, and unsafe. remove setters and use a parameter list in the constructor? or an init function2222
  const DummyDataHandle& DummyData() const;

  void setDummyData(DummyDataHandle value);


  // COLIN: I'd make that a true const method, and would set m_container in prepareAFterRead. What if the user doesn't call that?
  bool isAvailable() const; // precheck whether the pointee actually exists
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

private:
  ReferencingDataHandle(int index, int containerID,  std::vector<ReferencingData>* container);
  int m_index;
  int m_containerID;
  // COLIN: after reading, the transient m_container address must be taken from the registry using the persistent m_containerID and set. This seems to happen in isAvailable... why not in prepareAfterRead?
  mutable std::vector<ReferencingData>* m_container; //! transient
  albers::Registry* m_registry; //! transient
//  bool _retrieveData();

};

#endif
