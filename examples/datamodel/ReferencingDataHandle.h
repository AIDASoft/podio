#ifndef ReferencingDataHANDLE_H
#define ReferencingDataHANDLE_H
#include "ReferencingData.h"
#include "DummyDataHandle.h"
#include <vector>
//forward declaration of ReferencingData container
class ReferencingDataCollection;

namespace albers {
  class Registry;
}

class ReferencingDataHandle {

  friend ReferencingDataCollection;

public:

 ReferencingDataHandle(){};
 
  // COLIN: too painful to call each setter one by one, and unsafe. remove setters and use a parameter list in the constructor? or an init function2222
  const DummyDataHandle& DummyData() const;

  void setDummyData(DummyDataHandle value);


  bool isAvailable() const; // precheck whether the pointee actually exists
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

private:
  // COLIN: seems containerID means nothing here anyway->remove? 
  ReferencingDataHandle(int index, int containerID,  std::vector<ReferencingData>* container);
  int m_index; 
  int m_containerID;
  mutable std::vector<ReferencingData>* m_container; //! transient
  albers::Registry* m_registry; //! transient
//  bool _retrieveData();
  
};

#endif
