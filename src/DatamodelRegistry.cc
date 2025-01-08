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

size_t DatamodelRegistry::registerDatamodel(std::string name, std::string_view definition,
                                            const podio::RelationNameMapping& relationNames) {
  const auto it = std::ranges::find(m_definitions, name, &decltype(m_definitions)::value_type::first);

  if (it == m_definitions.cend()) {
    int index = m_definitions.size();
    m_definitions.emplace_back(std::move(name), definition);

    for (const auto& [typeName, relations, vectorMembers] : relationNames) {
      m_relations.emplace(typeName, RelationNames{relations, vectorMembers});
    }

    return index;
  }

  // TODO: Output?
  return std::ranges::distance(m_definitions.cbegin(), it);
}

size_t DatamodelRegistry::registerDatamodel(std::string name, std::string_view definition,
                                            const podio::RelationNameMapping& relationNames,
                                            podio::version::Version version) {
  auto index = registerDatamodel(name, definition, relationNames);
  m_datamodelVersions.emplace(std::move(name), version);
  return index;
}

const std::string_view DatamodelRegistry::getDatamodelDefinition(std::string_view name) const {
  const auto it = std::ranges::find(m_definitions, name, &decltype(m_definitions)::value_type::first);
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

RelationNames DatamodelRegistry::getRelationNames(std::string_view typeName) const {
  const static std::vector<std::string_view> emptyVec{};
  if (typeName.substr(0, 24) == "podio::UserDataCollection") {
    return {emptyVec, emptyVec};
  }

  if (typeName.substr(0, 11) == "podio::Link") {
    static constexpr auto fromName = "from";
    static constexpr auto toName = "to";
    const static std::vector<std::string_view> relationNames = {fromName, toName};
    return {relationNames, emptyVec};
  }

  // Strip Collection if necessary
  if (typeName.size() > 10 && typeName.substr(typeName.size() - 10) == "Collection") {
    typeName = typeName.substr(0, typeName.size() - 10);
  }

  if (const auto it = m_relations.find(typeName); it != m_relations.end()) {
    return it->second;
  }

  return {emptyVec, emptyVec};
}

std::optional<podio::version::Version> DatamodelRegistry::getDatamodelVersion(const std::string& name) const {
  if (const auto it = m_datamodelVersions.find(name); it != m_datamodelVersions.end()) {
    return it->second;
  }
  return std::nullopt;
}

} // namespace podio
