#include "podio/DatamodelRegistry.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string_view>

namespace podio {
const DatamodelRegistry& DatamodelRegistry::instance() {
  return mutInstance();
}

DatamodelRegistry& DatamodelRegistry::mutInstance() {
  static DatamodelRegistry registryInstance;
  return registryInstance;
}

size_t DatamodelRegistry::registerDatamodel(std::string name, std::string_view definition) {
  const auto it = std::find_if(m_definitions.cbegin(), m_definitions.cend(),
                               [&name](const auto& kvPair) { return kvPair.first == name; });

  if (it == m_definitions.cend()) {
    int index = m_definitions.size();
    m_definitions.emplace_back(name, definition);
    return index;
  }

  // TODO: Output?
  return std::distance(m_definitions.cbegin(), it);
}

const std::string_view DatamodelRegistry::getDatamodelDefinition(std::string_view name) const {
  const auto it = std::find_if(m_definitions.cbegin(), m_definitions.cend(),
                               [&name](const auto& kvPair) { return kvPair.first == name; });
  if (it == m_definitions.cend()) {
    std::cerr << "PODIO WARNING: Cannot find the definition for the EDM with the name " << name << std::endl;
    static constexpr std::string_view emptyDef = "{}"; // valid empty JSON
    return emptyDef;
  }

  return it->second;
}

const std::string_view DatamodelRegistry::getDatamodelDefinition(size_t index) const {
  if (index >= m_definitions.size()) {
    std::cerr << "PODIO WARNING: Cannot find the definition for the EDM with the index " << index << std::endl;
    static constexpr std::string_view emptyDef = "{}"; // valid empty JSON
    return emptyDef;
  }

  return m_definitions[index].second;
}

const std::string& DatamodelRegistry::getDatamodelName(size_t index) const {
  if (index >= m_definitions.size()) {
    std::cout << "PODIO WARNING: Cannot find the name of the EDM with the index " << index << std::endl;
    static const std::string emptyName = "";
    return emptyName;
  }
  return m_definitions[index].first;
}

} // namespace podio
