#ifndef JetHANDLE_H
#define JetHANDLE_H
#include "datamodel/Jet.h"
#include "datamodel/LorentzVector.h"

#include <vector>

// 
// author: C. Bernet, B. Hegner

//forward declaration of Jet container
class JetCollection;

namespace albers {
  class Registry;
}

class JetHandle {

  friend class JetCollection;

public:

JetHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  JetHandle() = default;

  const LorentzVector& P4() const;

  void setP4(LorentzVector value);



  bool isAvailable() const; // precheck whether the pointee actually exists
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

  /// equality operator (true if both the index and the container ID are equal)
  bool operator==(const JetHandle& other) const {
       return (m_index==other.m_index) && (other.m_containerID==other.m_containerID);
  }

  /// less comparison operator, so that Handles can be e.g. stored in sets.
  friend bool operator< (const JetHandle& p1,
			 const JetHandle& p2 );

private:
  JetHandle(int index, int containerID,  std::vector<Jet>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<Jet>* m_container; //! transient
  albers::Registry* m_registry; //! transient
//  bool _retrieveData();
  // members to support 1-to-N relations
  

};

#endif
