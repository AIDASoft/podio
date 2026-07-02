#ifndef PODIO_DETAIL_FILTERHOOKREGISTRY_H
#define PODIO_DETAIL_FILTERHOOKREGISTRY_H

#include "podio/detail/FilterHooks.h"

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

namespace podio {
namespace detail {

  /// Process-wide registry of per-datatype FilterHooks, keyed by datatype name
  /// (CollectionBase::getValueTypeName()). Mirrors CollectionBufferFactory: it is
  /// a singleton living in the core podio library and is populated when a
  /// datamodel library is loaded, so registration is unambiguous across shared
  /// libraries. Registration happens single-threaded at load time; lookups are
  /// read-only afterwards.
  class FilterHookRegistry {
  public:
    FilterHookRegistry(const FilterHookRegistry&) = delete;
    FilterHookRegistry& operator=(const FilterHookRegistry&) = delete;
    FilterHookRegistry(FilterHookRegistry&&) = delete;
    FilterHookRegistry& operator=(FilterHookRegistry&&) = delete;
    ~FilterHookRegistry() = default;

    /// Mutable instance, used for registration during library loading.
    static FilterHookRegistry& mutInstance();
    /// Const instance, used to look up hooks at filter time.
    static const FilterHookRegistry& instance();

    /// Register the hooks for a datatype.
    void registerHooks(const std::string& typeName, std::unique_ptr<FilterHooks> hooks);

    /// @returns the hooks for @p typeName, or nullptr if none are registered.
    const FilterHooks* getHooks(std::string_view typeName) const;

  private:
    FilterHookRegistry() = default;
    std::unordered_map<std::string, std::unique_ptr<FilterHooks>> m_hooks{};
  };

  /// Convenience used by generated registration code. Returns true so it can
  /// initialize a namespace-scope bool.
  inline bool registerFilterHooks(const std::string& typeName, std::unique_ptr<FilterHooks> hooks) {
    FilterHookRegistry::mutInstance().registerHooks(typeName, std::move(hooks));
    return true;
  }

} // namespace detail
} // namespace podio

#endif // PODIO_DETAIL_FILTERHOOKREGISTRY_H
