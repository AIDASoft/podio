// albers specific includes
#include "albers/ICollectionProvider.h"
#include "albers/Registry.h"

#include <cassert>

namespace albers {

  void Registry::doGetDataAddressFromID(int ID, void*& address) const {
    //std::lock_guard<std::recursive_mutex> lock(m_mutex);
    auto result = std::find(begin(m_collectionIDs), end(m_collectionIDs), ID);
    if (result == end(m_collectionIDs)){
      std::cout << "WARNING: Collection with ID " << ID << " not found." << std::endl;
      address = nullptr;
    } else {
      auto index = result - m_collectionIDs.begin();
      address = m_addresses[index];
      if (address == nullptr){
  // this is needed to set the address of handles embedded in other objects
   std::cout << "On demand reading of container with ID " << ID << std::endl;
        address = m_collectionProvider->getBuffer(ID);
      }
    }
  }

  void Registry::print() const {
    //std::lock_guard<std::recursive_mutex> lock(m_mutex);
    std::cout<<"Registry"<<std::endl;
    for(unsigned i=0; i<m_names.size(); ++i ) {
      std::cout<<"\t"
         <<m_names[i] << " : " << m_collectionIDs[i] <<std::endl;
    }
  }
} // namespace
