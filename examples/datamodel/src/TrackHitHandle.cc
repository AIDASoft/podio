// albers specific includes
#include "albers/Registry.h"
#include "albers/Reader.h"

// datamodel specific includes
#include "datamodel/TrackHit.h"
#include "datamodel/TrackHitCollection.h"



bool  TrackHitHandle::isAvailable() const {
  if (m_container != nullptr) {
    return true;
  }
  return false;
}

void TrackHitHandle::prepareForWrite(const albers::Registry* registry){
  m_containerID = registry->getIDFromPODAddress(m_container);
}

void TrackHitHandle::prepareAfterRead(albers::Registry* registry){
  m_registry = registry;
  m_registry->getPODAddressFromID(m_containerID,m_container);
}


TrackHitHandle::TrackHitHandle(int index, int containerID, std::vector<TrackHit>* container) :
  m_index(index),
  m_containerID(containerID),
  m_container(container)
{}


bool operator< (const TrackHitHandle& p1, const TrackHitHandle& p2 ) {
  if( p1.m_containerID == p2.m_containerID ) {
    return p1.m_index < p2.m_index;
  }
  else {
    return p1.m_containerID < p2.m_containerID;
  }
}
