#ifndef PODIO_IO_UTILS_H // NOLINT(llvm-header-guard): internal headers confuse clang-tidy
#define PODIO_IO_UTILS_H // NOLINT(llvm-header-guard): internal headers confuse clang-tidy

#include "podio/DatamodelRegistry.h"
#include "podio/utilities/DatamodelRegistryIOHelpers.h"

#include <iostream>
#include <string>
#include <vector>

namespace podio::io_utils {

/// Check and verify that all EDM (verions) that have been read from file are
/// readable
///
/// Effectively this boils down to making sure that the EDM versions on file are
/// not newer than the ones that have been loaded dynamically.
///
/// Returns all warnings that have been found
inline std::vector<std::string> checkEDMVersionsReadable(const podio::DatamodelDefinitionHolder& fileEdms) {
  std::vector<std::string> warnings{};
  for (const auto& edmName : fileEdms.getAvailableDatamodels()) {
    // There is no way we get an empty optional here
    const auto fileSchemaVersion = fileEdms.getSchemaVersion(edmName).value();
    const auto envSchemaVersion = podio::DatamodelRegistry::instance().getSchemaVersion(edmName);

    if (!envSchemaVersion) {
      warnings.emplace_back("EDM '" + edmName +
                            "' exists in file but does not seem to be loaded from the environment by podio");
    }
    if (envSchemaVersion.value() < fileSchemaVersion) {
      warnings.emplace_back("EDM '" + edmName + "' exists in file with schema version " +
                            std::to_string(fileSchemaVersion) + " but podio loaded schema version " +
                            std::to_string(envSchemaVersion.value()) + " from the environment");
    }
  }

  return warnings;
}
} // namespace podio::io_utils

#endif
