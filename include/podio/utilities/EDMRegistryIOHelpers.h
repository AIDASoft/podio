#ifndef PODIO_UTILITIES_EDMREGISTRYIOHELPERS_H
#define PODIO_UTILITIES_EDMREGISTRYIOHELPERS_H

#include "podio/CollectionBase.h"
#include "podio/EDMDefinitionRegistry.h"

#include <set>
#include <string>
#include <tuple>
#include <vector>

namespace podio {

/**
 * Helper class (mixin) to collect the EDM (JSON) definitions that should be
 * written.
 */
class EDMDefinitionCollector {
public:
  /// Register the EDM where this collection is from to be written
  void registerEDMDef(const podio::CollectionBase* coll, const std::string& name);

  /// Get all the names and JSON definitions that need to be written
  std::vector<std::tuple<std::string, std::string>> getEDMDefinitionsToWrite() const;

private:
  std::set<size_t> m_edmDefRegistryIdcs{}; ///< The indices in the EDM definition registry that need to be written
};

/**
 * Helper class (mixin) to hold and provide the EDM (JSON) definitions for
 * reader classes.
 */
class EDMDefinitionHolder {
public:
  /**
   * Get the EDM definition for the given EDM name. Returns an empty model
   * definition if no model is stored under the given name.
   */
  const std::string_view getEDMDefinition(const std::string& edmName) const;

protected:
  std::vector<std::tuple<std::string, std::string>> m_availEDMDefs{};
};

} // namespace podio

#endif // PODIO_UTILITIES_EDMREGISTRYIOHELPERS_H
