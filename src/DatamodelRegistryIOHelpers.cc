#include "podio/utilities/DatamodelRegistryIOHelpers.h"
#include "podio/CollectionBase.h"
#include "podio/DatamodelRegistry.h"

#include <algorithm>
#include <iostream>
#include <ranges>

namespace podio {

void DatamodelDefinitionCollector::registerDatamodelDefinition(const podio::CollectionBase* coll,
                                                               const std::string& name) {
  const auto edmIndex = coll->getDatamodelRegistryIndex();
  if (edmIndex == DatamodelRegistry::NoDefinitionAvailable) {
    std::cerr << "No EDM definition available for collection " << name << std::endl;
  } else {
    if (edmIndex != DatamodelRegistry::NoDefinitionNecessary) {
      m_edmDefRegistryIdcs.insert(edmIndex);
    }
  }
}

std::vector<std::tuple<std::string, std::string>> DatamodelDefinitionCollector::getDatamodelDefinitionsToWrite() const {
  const auto& registry = DatamodelRegistry::instance();
  using Definition = std::tuple<std::string, std::string>;
  const auto getDefinition = [&registry](auto index) {
    return Definition(registry.getDatamodelName(index), registry.getDatamodelDefinition(index));
  };
  return m_edmDefRegistryIdcs | std::views::transform(getDefinition) | std::ranges::to<std::vector>();
}

const std::string_view DatamodelDefinitionHolder::getDatamodelDefinition(std::string_view name) const {
  const auto it =
      std::ranges::find_if(m_availEDMDefs, [&name](const auto& entry) { return std::get<0>(entry) == name; });

  if (it != m_availEDMDefs.cend()) {
    return std::get<1>(*it);
  }

  return "{}";
}

std::vector<std::string> DatamodelDefinitionHolder::getAvailableDatamodels() const {
  return m_availEDMDefs | std::views::elements<0> | std::ranges::to<std::vector>();
}

std::optional<podio::version::Version> DatamodelDefinitionHolder::getDatamodelVersion(std::string_view name) const {
  const auto it = std::find_if(m_edmVersions.begin(), m_edmVersions.end(),
                               [&name](const auto& entry) { return std::get<0>(entry) == name; });
  if (it != m_edmVersions.end()) {
    return std::get<1>(*it);
  }

  return std::nullopt;
}

} // namespace podio
