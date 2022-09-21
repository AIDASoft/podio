// podio specific includes
#include "podio/CollectionIDTable.h"
#include <algorithm>
#include <iostream>

namespace podio {

CollectionIDTable::CollectionIDTable(): m_collectionIDs(), m_names(), m_mutex(std::make_unique<std::mutex>()) {

}

const std::string CollectionIDTable::name(int ID) const {
  std::lock_guard<std::mutex> lock(*m_mutex);
  const auto result = std::find(begin(m_collectionIDs), end(m_collectionIDs), ID);
  const auto index = std::distance(m_collectionIDs.begin(), result);
  return m_names[index];
}

int CollectionIDTable::collectionID(const std::string& name) const {
  std::lock_guard<std::mutex> lock(*m_mutex);
  const auto result = std::find(begin(m_names), end(m_names), name);
  const auto index = std::distance(m_names.begin(), result);
  return m_collectionIDs[index];
}

void CollectionIDTable::print() const {
  std::lock_guard<std::mutex> lock(*m_mutex);
  std::cout << "CollectionIDTable" << std::endl;
  for (unsigned i = 0; i < m_names.size(); ++i) {
    std::cout << "\t" << m_names[i] << " : " << m_collectionIDs[i] << std::endl;
  }
}

bool CollectionIDTable::present(const std::string& name) const {
  std::lock_guard<std::mutex> lock(*m_mutex);
  const auto result = std::find(begin(m_names), end(m_names), name);
  return result != end(m_names);
}

int CollectionIDTable::add(const std::string& name) {
  std::lock_guard<std::mutex> lock(*m_mutex);
  const auto result = std::find(begin(m_names), end(m_names), name);
  int ID = 0;
  if (result == m_names.end()) {
    m_names.emplace_back(name);
    ID = m_names.size();
    m_collectionIDs.emplace_back(ID);
  } else {
    const auto index = std::distance(m_names.begin(), result);
    ID = m_collectionIDs[index];
  }
  return ID;
}

} // namespace podio
