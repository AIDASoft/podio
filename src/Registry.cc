// albers specific includes
#include "albers/Reader.h"
#include "albers/Registry.h"

namespace albers {

  void Registry::doGetPODAddressFromID(unsigned ID, void*& address) const {
    auto result = std::find(begin(m_collectionIDs), end(m_collectionIDs), ID);
    if (result == end(m_collectionIDs)){
      address = nullptr;
    } else {
      auto index = result - m_collectionIDs.begin();
      address = m_addresses[index];
      if (address == nullptr){
	// this is needed to set the address of handles embedded in other objects
	std::cout << "On demand reading of container with ID " << ID << std::endl;
        address = m_reader->getBuffer(ID);
      }
    }
  }

} // namespace
