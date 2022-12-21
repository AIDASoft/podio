#include "podio/utilities/EDMRegistryIOHelpers.h"
#include <iterator>

namespace podio {

void EDMDefinitionCollector::registerEDMDef(const podio::CollectionBase* coll, const std::string& name) {
  const auto edmIndex = coll->getDefinitionRegistryIndex();
  if (edmIndex == EDMDefinitionRegistry::NoDefinitionAvailable) {
    std::cerr << "No EDM definition available for collection " << name << std::endl;
  } else {
    if (edmIndex != EDMDefinitionRegistry::NoDefinitionNecessary) {
      m_edmDefRegistryIdcs.insert(edmIndex);
    }
  }
}

std::vector<std::tuple<std::string, std::string>> EDMDefinitionCollector::getEDMDefinitionsToWrite() const {
  std::vector<std::tuple<std::string, std::string>> edmDefinitions;
  edmDefinitions.reserve(m_edmDefRegistryIdcs.size());
  for (const auto& index : m_edmDefRegistryIdcs) {
    const auto& edmRegistry = podio::EDMDefinitionRegistry::instance();
    edmDefinitions.emplace_back(edmRegistry.getEDMName(index), edmRegistry.getDefinition(index));
  }

  return edmDefinitions;
}

const std::string_view EDMDefinitionHolder::getEDMDefinition(const std::string& edmName) const {
  const auto it = std::find_if(m_availEDMDefs.cbegin(), m_availEDMDefs.cend(),
                               [&edmName](const auto& entry) { return std::get<0>(entry) == edmName; });

  if (it != m_availEDMDefs.cend()) {
    return std::get<1>(*it);
  }

  return "{}";
}

std::vector<std::string> EDMDefinitionHolder::getAvailableEDMDefinitions() const {
  std::vector<std::string> defs{};
  defs.reserve(m_availEDMDefs.size());
  std::transform(m_availEDMDefs.cbegin(), m_availEDMDefs.cend(), std::back_inserter(defs),
                 [](const auto& elem) { return std::get<0>(elem); });

  return defs;
}

} // namespace podio
