#ifndef PODIO_EDMDEFINITIONREGISTRY_H
#define PODIO_EDMDEFINITIONREGISTRY_H

#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace podio {
class EDMDefinitionRegistry {
public:
  static EDMDefinitionRegistry& instance();
  ~EDMDefinitionRegistry() = default;
  EDMDefinitionRegistry(const EDMDefinitionRegistry&) = delete;
  EDMDefinitionRegistry& operator=(const EDMDefinitionRegistry&) = delete;
  EDMDefinitionRegistry(EDMDefinitionRegistry&&) = delete;
  EDMDefinitionRegistry& operator=(const EDMDefinitionRegistry&&) = delete;

  /// Dedicated index value for collections that don't need a definition (e.g. UserDataCollection)
  static constexpr size_t NoDefinitionNecessary = -1;
  /// Dedicated index value for error checking, used to default init the generated RegistryIndex
  static constexpr size_t NoDefinitionAvailable = -2;

  /**
   * Get the definition (in JSON format) of the EDM with the given edm_name. If
   * no EDM under the given name can be found, an empty model definition is
   * returned
   */
  const std::string_view getDefinition(std::string_view edm_name) const;

  /**
   * Get the defintion (in JSON format) of the EDM wth the given index. If no
   * EDM is found under the given index, an empty model definition is returned.
   */
  const std::string_view getDefinition(size_t index) const;

  /**
   * Register a definition and return the index in the registry. If a definition
   * already exists under the given name, then the index of the existing
   * definition is returned
   */
  size_t registerEDM(std::string name, std::string_view definition);

private:
  EDMDefinitionRegistry() = default;
  /// The stored definitions
  std::vector<std::pair<std::string, std::string_view>> m_definitions{};
};
} // namespace podio

#endif // PODIO_EDMDEFINITIONREGISTRY_H
