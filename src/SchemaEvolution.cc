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

  if (const auto typeIt = m_evolutionFuncs.find(collType); typeIt != m_evolutionFuncs.end()) {
    const auto& versionMap = typeIt->second;
    // The current schema version is defined by the number of evolution functions we have
    const auto currentVersion = versionMap.size();
    std::cerr << "PODIO WARNING: evolveBuffers " << collType << " current " << currentVersion << " buffer version "
              << fromVersion << std::endl;

    if (currentVersion == fromVersion) {
      return oldBuffers; // Nothing to do here
    }

    if (fromVersion < currentVersion) {
      return versionMap[fromVersion - 1](oldBuffers, fromVersion);
    }

    std::cerr
        << "PODIO WARNING: evolveBuffers called with fromVersion that is greater than the current schema version for "
        << collType << std::endl;
    // TODO: exception?
    return oldBuffers;
  }

  std::cerr << "PODIO WARNING: evolveBuffers has no knowledge of how to evolve buffers for " << collType << std::endl;
  // TODO: exception
  return oldBuffers;
}

void SchemaEvolution::registerEvolutionFunc(const std::string& collType, SchemaVersionT fromVersion,
                                            const EvolutionFuncT& evolutionFunc, Priority priority) {
  auto typeIt = m_evolutionFuncs.find(collType);
  if (typeIt != m_evolutionFuncs.end()) {
    auto& versionMap = typeIt->second;

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

  } else {
    // We do not know about this type yet, so we create an entirely new map
    // and populate all versions with this evolution function
    VersionMapT versionMap;
    versionMap.reserve(fromVersion);
    for (size_t i = 0; i < fromVersion; ++i) {
      versionMap.emplace_back(evolutionFunc);
    }

    m_evolutionFuncs.emplace(collType, std::move(versionMap));
  }
}

void SchemaEvolution::registerEvolutionFunc(const std::string& collType, NoSchemaEvolutionNecessaryT) {
  auto typeIt = m_evolutionFuncs.find(collType);
  if (typeIt != m_evolutionFuncs.end()) {
    std::runtime_error(
        "Cannot mark a type for not needing schema evolution, if it already has schema evolution functions defined");
  }

  // TODO: How to guard this agains accidental overwriting later?
  m_evolutionFuncs.emplace(collType, VersionMapT{});
}

} // namespace podio
