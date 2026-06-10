#ifndef PODIO_FRAMEFILTER_H
#define PODIO_FRAMEFILTER_H

#include "podio/CollectionBase.h"
#include "podio/Frame.h"
#include "podio/ObjectID.h"
#include "podio/detail/FilterHookRegistry.h"
#include "podio/detail/FilterHooks.h"

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace podio {

namespace detail {
  /// Deduce the (decayed) argument type of a unary predicate's operator().
  template <typename T>
  struct FilterPredArg;
  template <typename R, typename C, typename A>
  struct FilterPredArg<R (C::*)(A) const> {
    using type = std::decay_t<A>;
  };
  template <typename R, typename C, typename A>
  struct FilterPredArg<R (C::*)(A)> {
    using type = std::decay_t<A>;
  };
  template <typename Pred>
  using FilterPredArgT = typename FilterPredArg<decltype(&std::decay_t<Pred>::operator())>::type;
} // namespace detail

/// Filters the collections of a Frame into a new Frame, rewiring all relations.
///
/// Two orthogonal axes control the result:
///  - Retention (per collection): keep-all (default), a predicate (`keep`), or
///    keep-if-referenced (`keepReferenced`, i.e. drop objects that no surviving
///    object points at).
///  - Edge policy (per relation): a dangling reference is left unset by default;
///    `cascade("Type.relation")` instead removes the object holding the relation
///    when its target is removed.
///
/// All collections that participate in the relation graph are rebuilt so the
/// output Frame is internally consistent.
class FrameFilter {
public:
  explicit FrameFilter(const podio::Frame& frame) : m_frame(frame) {
  }

  /// Keep objects of the named collection for which @p predicate is true.
  /// The collection type is deduced from the predicate's argument type.
  template <typename Pred>
  FrameFilter& keep(const std::string& name, Pred predicate) {
    using ValueT = detail::FilterPredArgT<Pred>;
    using CollT = typename ValueT::collection_type;
    m_predicates[name] = [predicate = std::move(predicate)](const podio::CollectionBase& coll, std::size_t i) {
      return predicate(static_cast<const CollT&>(coll)[i]);
    };
    return *this;
  }

  /// Keep objects of the named collection only if a surviving object references
  /// them (orphan sweep, shared-aware across collections).
  FrameFilter& keepReferenced(const std::string& name) {
    m_keepReferenced.insert(name);
    return *this;
  }

  /// Treat the named relation ("Type.relation") as mandatory: if its target is
  /// removed, remove the object holding it (integrity cascade).
  FrameFilter& cascade(const std::string& qualifiedRelation) {
    m_cascade.insert(qualifiedRelation);
    return *this;
  }

  /// Run the filter and return a new, fully-rewired Frame.
  podio::Frame run() const {
    enum Mode { KeepAll, Predicate, Referenced };
    struct CollInfo {
      std::string name;
      const podio::CollectionBase* coll;
      const detail::FilterHooks* hooks;
      std::string type;
      uint32_t id;
      std::size_t size;
      Mode mode;
      std::vector<char> killed;
      std::vector<char> marked;
    };

    std::vector<CollInfo> colls;
    std::unordered_map<uint32_t, std::size_t> byId;
    for (const auto& name : m_frame.getAvailableCollections()) {
      const auto* coll = m_frame.get(name);
      const auto type = std::string(coll->getValueTypeName());
      const auto* hooks = detail::FilterHookRegistry::instance().getHooks(type);
      if (!hooks) {
        throw std::runtime_error("podio::FrameFilter: no filter hooks registered for type '" + type +
                                 "' (collection '" + name + "')");
      }
      Mode mode = KeepAll;
      if (m_predicates.count(name)) {
        mode = Predicate;
      } else if (m_keepReferenced.count(name)) {
        mode = Referenced;
      }
      const auto size = coll->size();
      byId[coll->getID()] = colls.size();
      colls.push_back({name, coll, hooks, type, coll->getID(), size, mode,
                       std::vector<char>(size, 0), std::vector<char>(size, 0)});
    }

    // Resolve a relation target to (collection slot, object index).
    const auto locate = [&](podio::ObjectID target) -> std::pair<long, std::size_t> {
      if (target.index < 0) {
        return {-1, 0};
      }
      const auto it = byId.find(target.collectionID);
      return it == byId.end() ? std::pair<long, std::size_t>{-1, 0}
                              : std::pair<long, std::size_t>{static_cast<long>(it->second),
                                                             static_cast<std::size_t>(target.index)};
    };

    // Phase 0: seed removals from predicates.
    for (auto& ci : colls) {
      if (ci.mode == Predicate) {
        const auto& pred = m_predicates.at(ci.name);
        for (std::size_t i = 0; i < ci.size; ++i) {
          ci.killed[i] = pred(*ci.coll, i) ? 0 : 1;
        }
      }
    }

    // Phase 1: integrity-cascade closure (monotonic). If a cascade relation's
    // target is killed, the object holding it is killed too.
    for (bool changed = true; changed;) {
      changed = false;
      for (auto& ci : colls) {
        for (std::size_t i = 0; i < ci.size; ++i) {
          if (ci.killed[i]) {
            continue;
          }
          bool kill = false;
          ci.hooks->edges(*ci.coll, i, [&](std::string_view rel, podio::ObjectID target) {
            if (kill || !m_cascade.count(ci.type + "." + std::string(rel))) {
              return;
            }
            const auto [k, idx] = locate(target);
            if (k >= 0 && colls[k].killed[idx]) {
              kill = true;
            }
          });
          if (kill) {
            ci.killed[i] = 1;
            changed = true;
          }
        }
      }
    }

    // Phase 2: reachability mark (monotonic). Roots are surviving objects in
    // non-referenced collections; marking flows forward and decides the fate of
    // keep-referenced collections.
    for (bool changed = true; changed;) {
      changed = false;
      for (std::size_t k = 0; k < colls.size(); ++k) {
        auto& ci = colls[k];
        for (std::size_t i = 0; i < ci.size; ++i) {
          if (ci.killed[i] || (ci.mode == Referenced && !ci.marked[i])) {
            continue; // not a surviving source (yet)
          }
          ci.hooks->edges(*ci.coll, i, [&](std::string_view, podio::ObjectID target) {
            const auto [tk, tidx] = locate(target);
            if (tk >= 0 && colls[tk].mode == Referenced && !colls[tk].killed[tidx] && !colls[tk].marked[tidx]) {
              colls[tk].marked[tidx] = 1;
              changed = true;
            }
          });
        }
      }
    }

    // Phase 3: sweep unreferenced objects in keep-referenced collections.
    for (auto& ci : colls) {
      if (ci.mode == Referenced) {
        for (std::size_t i = 0; i < ci.size; ++i) {
          if (!ci.marked[i]) {
            ci.killed[i] = 1;
          }
        }
      }
    }

    // Phase 4: clone survivors, build the global remap, then rewire.
    std::vector<std::vector<std::size_t>> survivors(colls.size());
    std::vector<std::unique_ptr<podio::CollectionBase>> outColls(colls.size());
    detail::FilterRemap remap;
    for (std::size_t k = 0; k < colls.size(); ++k) {
      auto& ci = colls[k];
      for (std::size_t i = 0; i < ci.size; ++i) {
        if (!ci.killed[i]) {
          survivors[k].push_back(i);
        }
      }
      outColls[k] = ci.hooks->cloneSurvivors(*ci.coll, survivors[k]);
      for (std::size_t j = 0; j < survivors[k].size(); ++j) {
        podio::ObjectID original;
        original.index = static_cast<int>(survivors[k][j]);
        original.collectionID = ci.id;
        remap.add(original, outColls[k].get(), j);
      }
    }
    for (std::size_t k = 0; k < colls.size(); ++k) {
      colls[k].hooks->rewire(*colls[k].coll, survivors[k], *outColls[k], remap);
    }

    // Phase 5: assemble the output Frame.
    podio::Frame out;
    for (std::size_t k = 0; k < colls.size(); ++k) {
      out.put(std::move(outColls[k]), colls[k].name);
    }
    return out;
  }

private:
  const podio::Frame& m_frame;
  std::unordered_map<std::string, std::function<bool(const podio::CollectionBase&, std::size_t)>> m_predicates{};
  std::unordered_set<std::string> m_keepReferenced{};
  std::unordered_set<std::string> m_cascade{};
};

} // namespace podio

#endif // PODIO_FRAMEFILTER_H
