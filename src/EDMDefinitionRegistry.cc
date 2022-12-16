#include "podio/EDMDefinitionRegistry.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string_view>

namespace podio {
EDMDefinitionRegistry& EDMDefinitionRegistry::instance() {
  static EDMDefinitionRegistry registryInstance;
  return registryInstance;
}

size_t EDMDefinitionRegistry::registerEDM(std::string name, std::string_view definition) {
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

const std::string_view EDMDefinitionRegistry::getDefinition(std::string_view edm_name) const {
  const auto it = std::find_if(m_definitions.cbegin(), m_definitions.cend(),
                               [&edm_name](const auto& kvPair) { return kvPair.first == edm_name; });

  // TODO: Output when not found
  return getDefinition(std::distance(m_definitions.cbegin(), it));
}

const std::string_view EDMDefinitionRegistry::getDefinition(size_t index) const {
  if (index >= m_definitions.size()) {
    static constexpr std::string_view emptyDef = "{}"; // valid empty JSON
    return emptyDef;
  }

  return m_definitions[index].second;
}

const std::string& EDMDefinitionRegistry::getEDMName(size_t index) const {
  if (index >= m_definitions.size()) {
    static const std::string emptyName = "";
    return emptyName;
  }
  return m_definitions[index].first;
}

} // namespace podio
