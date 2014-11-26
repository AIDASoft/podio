#ifndef ReferencingDataHANDLE_H
#define ReferencingDataHANDLE_H
#include "datamodel/ReferencingData.h"
#include "datamodel/DummyDataHandle.h"
#include <vector>
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

  friend class ReferencingDataCollection;

public:

ReferencingDataHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  ReferencingDataHandle() = default;

  const DummyDataHandle& Dummy() const;

  void setDummy(DummyDataHandle value);

  void addDummies(DummyDataHandle&);
  std::vector<DummyDataHandle>::const_iterator Dummies_begin() const;
  std::vector<DummyDataHandle>::const_iterator Dummies_end() const;


  bool isAvailable() const; // precheck whether the pointee actually exists
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

  /// equality operator (true if both the index and the container ID are equal)
  bool operator==(const ReferencingDataHandle& other) const {
       return (m_index==other.m_index) && (other.m_containerID==other.m_containerID);
  }

  /// less comparison operator, so that Handles can be e.g. stored in sets.
  friend bool operator< (const ReferencingDataHandle& p1,
			 const ReferencingDataHandle& p2 );

private:
  ReferencingDataHandle(int index, int containerID,  std::vector<ReferencingData>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<ReferencingData>* m_container; //! transient
  albers::Registry* m_registry; //! transient
//  bool _retrieveData();
  // members to support 1-to-N relations
  std::vector<DummyDataHandle>* m_Dummies; //! transient 


};

#endif
