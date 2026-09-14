#include "podio/detail/FilterHookRegistry.h"

namespace podio {
namespace detail {

FilterHookRegistry& FilterHookRegistry::mutInstance() {
  static FilterHookRegistry registry;
  return registry;
}

const FilterHookRegistry& FilterHookRegistry::instance() {
  return mutInstance();
}

void FilterHookRegistry::registerHooks(const std::string& typeName, std::unique_ptr<FilterHooks> hooks) {
  m_hooks[typeName] = std::move(hooks);
}

const FilterHooks* FilterHookRegistry::getHooks(std::string_view typeName) const {
  const auto it = m_hooks.find(std::string(typeName));
  return it == m_hooks.end() ? nullptr : it->second.get();
}

} // namespace detail
} // namespace podio
