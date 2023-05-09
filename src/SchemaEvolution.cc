#include "podio/SchemaEvolution.h"
#include "podio/CollectionBuffers.h"

#include <iostream>

namespace podio {

SchemaEvolution& SchemaEvolution::mutInstance() {
  static SchemaEvolution instance;
  return instance;
}

SchemaEvolution const& SchemaEvolution::instance() {
  return mutInstance();
}

podio::CollectionReadBuffers SchemaEvolution::evolveBuffers(podio::CollectionReadBuffers oldBuffers,
                                                            SchemaVersionT fromVersion,
                                                            const std::string& collType) const {
  if (const auto typeIt = m_versionMapIndices.find(collType); typeIt != m_versionMapIndices.end()) {
    const auto [currentVersion, mapIndex] = typeIt->second;
    if (fromVersion == currentVersion) {
      return oldBuffers; // Nothing to do here
    }

    const auto& typeEvolFuncs = m_evolutionFuncs[mapIndex];
    if (fromVersion < typeEvolFuncs.size() - 1) {
      // Do we need this check? In principle we could ensure at registration
      // time that this is always guaranteed
      return typeEvolFuncs[fromVersion - 1](oldBuffers, fromVersion);
    }
  }

  std::cerr << "PODIO WARNING: evolveBuffers has no knowledge of how to evolve buffers for " << collType << std::endl;
  // TODO: exception
  return oldBuffers;
}

void SchemaEvolution::registerEvolutionFunc(const std::string& collType, SchemaVersionT fromVersion,
                                            const EvolutionFuncT& evolutionFunc, Priority priority) {
  auto typeIt = m_versionMapIndices.find(collType);
  if (typeIt == m_versionMapIndices.end()) {
    std::cerr << "PODIO ERROR: trying to register a schema evolution function for " << collType
              << " which is not a type known to the schema evolution registry" << std::endl;
    return;
  }

  auto& [currentVersion, mapIndex] = typeIt->second;

  // If we do not have any evolution funcs yet, create the necessary mapping
  // structure and update the index
  if (typeIt->second.index == MapIndex::NoEvolutionAvailable) {
    mapIndex = m_evolutionFuncs.size();
    m_evolutionFuncs.emplace_back(EvolFuncVersionMapT{});
  }

  auto& versionMap = m_evolutionFuncs[mapIndex];
  const auto prevSize = versionMap.size();
  if (prevSize < fromVersion) {
    versionMap.resize(fromVersion);
    for (auto i = prevSize; i < fromVersion; ++i) {
      versionMap[i] = evolutionFunc;
    }
  } else {
    if (priority == Priority::UserDefined) {
      versionMap[fromVersion] = evolutionFunc;
    } else {
      std::cerr << "Not updating evolution function because priority is not UserDefined" << std::endl;
    }
  }
}

void SchemaEvolution::registerCurrentVersion(const std::string& collType, SchemaVersionT currentVersion) {
  if (auto typeIt = m_versionMapIndices.find(collType); typeIt != m_versionMapIndices.end()) {
    // TODO: warn about this? In principle all of this should only be called once
    typeIt->second.currentVersion = currentVersion;
  }

  m_versionMapIndices.emplace(collType, MapIndex{currentVersion, MapIndex::NoEvolutionAvailable});
}

} // namespace podio
