// albers specific includes
#include "albers/CollectionIDTable.h"
#include <iostream>

namespace albers {

  const std::string CollectionIDTable::name(int ID) const {
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    auto result = std::find(begin(m_collectionIDs), end(m_collectionIDs), ID);
    auto index = result - m_collectionIDs.begin();
    return m_names[index];
  }

  int CollectionIDTable::collectionID(const std::string& name) const {
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    auto result = std::find(begin(m_names), end(m_names), name);
    auto index = result - m_names.begin();
    return m_collectionIDs[index];
  }


  void CollectionIDTable::print() const {
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    std::cout<<"CollectionIDTable"<<std::endl;
    for(unsigned i=0; i<m_names.size(); ++i ) {
      std::cout<<"\t"
         <<m_names[i] << " : " << m_collectionIDs[i] <<std::endl;
    }
  }

  int CollectionIDTable::add(const std::string& name) {
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    auto result = std::find(begin(m_names), end(m_names), name);
    int ID = 0;
    if (result == m_names.end()) {
      m_names.emplace_back(name);
      ID = m_names.size();
      m_collectionIDs.emplace_back( ID );
    } else {
        auto index = result - m_names.begin();
        ID = m_collectionIDs[index];
    }
    return ID;
  }

} // namespace
