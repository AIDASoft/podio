#include "podio/utilities/DatamodelRegistryIOHelpers.h"

#include <algorithm>
#include <charconv>
#include <iterator>

namespace podio {
namespace detail {
  podio::SchemaVersionT extractSchemaVersion(const std::string_view definition) {
    // Extract schema_version from JSON definition without full parsing
    // Look for "schema_version": followed by a number
    constexpr std::string_view schemaVersionKey = "\"schema_version\":";
    if (auto pos = definition.find(schemaVersionKey); pos != std::string_view::npos) {
      pos += schemaVersionKey.length();
      // Skip whitespace
      while (pos < definition.length() && std::isspace(definition[pos])) {
        ++pos;
      }
      // Extract the number
      auto start = pos;
      while (pos < definition.length() && std::isdigit(definition[pos])) {
        ++pos;
      }
      if (pos > start) {
        // Convert substring to integer using std::from_chars for better error handling
        podio::SchemaVersionT schemaVersion = 0;
        auto result = std::from_chars(definition.data() + start, definition.data() + pos, schemaVersion);
        if (result.ec == std::errc{}) {
          return schemaVersion;
        }
      }
    }

    // Return 0 if no valid schema version found
    return 0;
  }
} // namespace detail

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
  std::vector<std::tuple<std::string, std::string>> edmDefinitions;
  edmDefinitions.reserve(m_edmDefRegistryIdcs.size());
  for (const auto& index : m_edmDefRegistryIdcs) {
    const auto& edmRegistry = podio::DatamodelRegistry::instance();
    edmDefinitions.emplace_back(edmRegistry.getDatamodelName(index), edmRegistry.getDatamodelDefinition(index));
  }

  return edmDefinitions;
}

const std::string_view DatamodelDefinitionHolder::getDatamodelDefinition(const std::string& name) const {
  const auto it =
      std::ranges::find_if(m_availEDMDefs, [&name](const auto& entry) { return std::get<0>(entry) == name; });

  if (it != m_availEDMDefs.cend()) {
    return std::get<1>(*it);
  }

  return "{}";
}

std::vector<std::string> DatamodelDefinitionHolder::getAvailableDatamodels() const {
  std::vector<std::string> defs{};
  defs.reserve(m_availEDMDefs.size());
  std::transform(m_availEDMDefs.cbegin(), m_availEDMDefs.cend(), std::back_inserter(defs),
                 [](const auto& elem) { return std::get<0>(elem); });

  return defs;
}

std::optional<podio::version::Version> DatamodelDefinitionHolder::getDatamodelVersion(const std::string& name) const {
  const auto it = std::find_if(m_edmVersions.begin(), m_edmVersions.end(),
                               [&name](const auto& entry) { return std::get<0>(entry) == name; });
  if (it != m_edmVersions.end()) {
    return std::get<1>(*it);
  }

  return std::nullopt;
}

std::optional<podio::SchemaVersionT> DatamodelDefinitionHolder::getSchemaVersion(const std::string& name) const {
  const auto definition = getDatamodelDefinition(name);
  if (definition != "{}") {
    return detail::extractSchemaVersion(definition);
  }
  return std::nullopt;
}

} // namespace podio
