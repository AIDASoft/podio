#ifndef DummyDataHANDLE_H
#define DummyDataHANDLE_H
#include "DummyData.h"

#include <vector>

// test data
// author: C. Bernet, B. Hegner

//forward declaration of DummyData container
class DummyDataCollection;

namespace albers {
  class Registry;
}

class DummyDataHandle {

  friend DummyDataCollection;

public:

DummyDataHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  DummyDataHandle() = default;

  // COLIN: too painful to call each setter one by one, and unsafe. remove setters and use a parameter list in the constructor? or an init function2222
  const int& Number() const;

  void setNumber(int value);


  // COLIN: I'd make that a true const method, and would set m_container in prepareAFterRead. What if the user doesn't call that?
  bool isAvailable() const; // precheck whether the pointee actually exists
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

private:
  DummyDataHandle(int index, int containerID,  std::vector<DummyData>* container);
  int m_index;
  int m_containerID;
  // COLIN: after reading, the transient m_container address must be taken from the registry using the persistent m_containerID and set. This seems to happen in isAvailable... why not in prepareAfterRead?
  mutable std::vector<DummyData>* m_container; //! transient
  albers::Registry* m_registry; //! transient
//  bool _retrieveData();

};

#endif
