// podio specific includes
#include "podio/CollectionIDTable.h"

#include "MurmurHash3.h"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <ranges>
#include <utility>

namespace podio {

CollectionIDTable::CollectionIDTable(std::vector<uint32_t>&& ids, std::vector<std::string>&& names) :
    m_collectionIDs(std::move(ids)), m_names(std::move(names)) {
  rebuildMaps();
}

CollectionIDTable::CollectionIDTable(const std::vector<uint32_t>& ids, const std::vector<std::string>& names) :
    m_collectionIDs(ids), m_names(names) {
  rebuildMaps();
}

std::mutex& CollectionIDTable::getMutex() const {
  if (!m_mutex) {
    m_mutex = std::make_unique<std::mutex>();
  }
  return *m_mutex;
}

void CollectionIDTable::rebuildMaps() const {
  m_nameToID.clear();
  m_idToIndex.clear();
  const auto n = std::min(m_collectionIDs.size(), m_names.size());
  m_nameToID.reserve(n);
  m_idToIndex.reserve(n);
  for (std::size_t i = 0; i < n; ++i) {
    m_nameToID.try_emplace(m_names[i], m_collectionIDs[i]);
    m_idToIndex.try_emplace(m_collectionIDs[i], i);
  }
}

void CollectionIDTable::ensureMapsPopulated() const {
  if ((m_nameToID.empty() && !m_names.empty()) || (m_idToIndex.empty() && !m_collectionIDs.empty())) {
    rebuildMaps();
  }
}

std::optional<const std::string> CollectionIDTable::name(uint32_t id) const {
  std::lock_guard<std::mutex> lock{getMutex()};
  ensureMapsPopulated();
  if (const auto it = m_idToIndex.find(id); it != m_idToIndex.end()) {
    return m_names[it->second];
  }
  return std::nullopt;
}

std::optional<uint32_t> CollectionIDTable::collectionID(std::string_view collectionName) const {
  std::lock_guard<std::mutex> lock{getMutex()};
  ensureMapsPopulated();
  if (const auto it = m_nameToID.find(collectionName); it != m_nameToID.end()) {
    return it->second;
  }
  return std::nullopt;
}

void CollectionIDTable::print() const {
  std::lock_guard<std::mutex> lock{getMutex()};
  std::cout << "CollectionIDTable" << std::endl;
  for (unsigned i = 0; i < m_names.size(); ++i) {
    std::cout << "\t" << m_names[i] << " : " << m_collectionIDs[i] << std::endl;
  }
}

bool CollectionIDTable::present(std::string_view collectionName) const {
  return collectionID(collectionName).has_value();
}

bool CollectionIDTable::present(uint32_t id) const {
  return name(id).has_value();
}

uint32_t CollectionIDTable::add(const std::string& collectionName) {
  std::lock_guard<std::mutex> lock{getMutex()};
  ensureMapsPopulated();
  if (const auto it = m_nameToID.find(collectionName); it != m_nameToID.end()) {
    return it->second;
  }

  uint32_t ID = 0;
  MurmurHash3_x86_32(collectionName.c_str(), collectionName.size(), 0, &ID);
  const auto index = m_names.size();
  m_names.emplace_back(collectionName);
  m_collectionIDs.emplace_back(ID);
  m_nameToID.try_emplace(m_names.back(), ID);
  m_idToIndex.try_emplace(ID, index);
  return ID;
}

} // namespace podio
