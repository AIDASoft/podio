#ifndef DummyDataHANDLE_H
#define DummyDataHANDLE_H
#include "DummyData.h"

#include <vector>
//forward declaration of DummyData container
class DummyDataCollection;

namespace albers {
  class Registry;
}

class DummyDataHandle {

  friend DummyDataCollection;

public:

 DummyDataHandle(){};
 
  // COLIN: too painful to call each setter one by one, and unsafe. remove setters and use a parameter list in the constructor? or an init function2222
  const int& Number() const;

  void setNumber(int value);


  bool isAvailable() const; // precheck whether the pointee actually exists
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

private:
  // COLIN: seems containerID means nothing here anyway->remove? 
  DummyDataHandle(int index, int containerID,  std::vector<DummyData>* container);
  int m_index; 
  int m_containerID;
  mutable std::vector<DummyData>* m_container; //! transient
  albers::Registry* m_registry; //! transient
//  bool _retrieveData();
  
};

#endif
