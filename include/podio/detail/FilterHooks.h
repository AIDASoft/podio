#ifndef PODIO_DETAIL_FILTERHOOKS_H
#define PODIO_DETAIL_FILTERHOOKS_H

#include "podio/ObjectID.h"

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace podio {
class CollectionBase;

namespace detail {

  /// Where an object ended up in the filtered output: the new collection and the
  /// index of the object within it.
  struct RemapTarget {
    podio::CollectionBase* collection{nullptr};
    std::size_t index{0};
  };

  /// Maps an object's original ObjectID to its location in the filtered output.
  /// A missing entry means the object was removed by the filter.
  class FilterRemap {
  public:
    void add(podio::ObjectID original, podio::CollectionBase* collection, std::size_t index) {
      m_map[original] = {collection, index};
    }

    /// @returns the new location of the object, or nullptr if it was removed
    const RemapTarget* find(podio::ObjectID original) const {
      const auto it = m_map.find(original);
      return it == m_map.end() ? nullptr : &it->second;
    }

  private:
    std::unordered_map<podio::ObjectID, RemapTarget> m_map{};
  };

  /// Resolve a relation target to the const handle it now has in the filtered
  /// output collection of type @p CollT, or std::nullopt if it was removed. Used
  /// by the generated rewire hooks.
  template <typename CollT>
  std::optional<typename CollT::value_type> resolveTarget(const FilterRemap& remap, podio::ObjectID target) {
    const auto* loc = remap.find(target);
    if (!loc) {
      return std::nullopt;
    }
    return static_cast<const CollT&>(*loc->collection)[loc->index];
  }

  /// Callback used to report a single outgoing relation edge of an object during
  /// the cascade/reachability passes: the relation name and the target ObjectID.
  using EdgeSink = std::function<void(std::string_view relationName, podio::ObjectID target)>;

  /// Type-erased, per-datatype operations the FrameFilter needs but which require
  /// concrete type knowledge (so they are generated, one implementation per type,
  /// and registered by type name). The FrameFilter only ever sees CollectionBase.
  struct FilterHooks {
    virtual ~FilterHooks() = default;

    /// Report the outgoing relation edges of object @p index in @p collection.
    virtual void edges(const podio::CollectionBase& collection, std::size_t index,
                       const EdgeSink& sink) const = 0;

    /// Clone the objects at @p survivors (in order) into a freshly allocated
    /// collection of the same type, copying data members but no relations.
    virtual std::unique_ptr<podio::CollectionBase>
    cloneSurvivors(const podio::CollectionBase& collection, const std::vector<std::size_t>& survivors) const = 0;

    /// Rewire the relations of the cloned survivors in @p output, resolving each
    /// relation target through @p remap. @p input is the original collection and
    /// @p survivors the same index list passed to cloneSurvivors (so output[j]
    /// corresponds to input[survivors[j]]).
    virtual void rewire(const podio::CollectionBase& input, const std::vector<std::size_t>& survivors,
                        podio::CollectionBase& output, const FilterRemap& remap) const = 0;
  };

  /// The process-wide registry of filter hooks, keyed by datatype name (as
  /// returned by CollectionBase::getTypeName()). Header-only so that generated
  /// datamodel code can register into it without a link-time dependency.
  inline std::unordered_map<std::string, std::unique_ptr<FilterHooks>>& filterHookRegistry() {
    static std::unordered_map<std::string, std::unique_ptr<FilterHooks>> registry;
    return registry;
  }

  /// Register the hooks for a datatype. Returns true so it can be used to
  /// initialize a namespace-scope bool in generated code.
  inline bool registerFilterHooks(std::string typeName, std::unique_ptr<FilterHooks> hooks) {
    filterHookRegistry()[std::move(typeName)] = std::move(hooks);
    return true;
  }

  /// @returns the hooks registered for @p typeName, or nullptr if none.
  inline const FilterHooks* getFilterHooks(std::string_view typeName) {
    auto& registry = filterHookRegistry();
    const auto it = registry.find(std::string(typeName));
    return it == registry.end() ? nullptr : it->second.get();
  }

} // namespace detail
} // namespace podio

#endif // PODIO_DETAIL_FILTERHOOKS_H
