#ifndef PODIO_UTILITIES_STRINGKEYMAP_H
#define PODIO_UTILITIES_STRINGKEYMAP_H

#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace podio {

namespace detail {

  /// Transparent hash for @c std::string keyed @c std::unordered_map that enables
  /// heterogeneous lookup with @c std::string_view (and @c const @c char*) without
  /// constructing a temporary @c std::string for each lookup. Must be used together
  /// with @c std::equal_to<> as the key equality comparator so that the same
  /// key-comparison transparency applies.
  struct TransparentStringHash {
    using is_transparent = void;
    // We only need the string_View overload because a string converts to that
    std::size_t operator()(std::string_view sv) const noexcept {
      return std::hash<std::string_view>{}(sv);
    }
  };

} // namespace detail

/// A convenience alias for @c std::unordered_map<std::string, Value> that
/// supports heterogeneous lookup.
///
/// Keys can be looked up (via @c find, @c count, @c contains, ...) using any
/// type that is implicitly convertible to @c std::string_view — most notably
/// @c std::string_view and plain string literals — without constructing a
/// temporary @c std::string. This is achieved by pairing @c
/// podio::detail::TransparentStringHash with @c std::equal_to<> (the
/// "transparent" equality comparator introduced in C++14).
///
/// @note Insertion operations (@c try_emplace, @c operator[], …) still require
/// a @c std::string key since that is the map's @c key_type. Pass
/// @c std::string(sv) explicitly when inserting from a @c std::string_view.
template <typename Value>
using StringKeyMap = std::unordered_map<std::string, Value, detail::TransparentStringHash, std::equal_to<>>;
} // namespace podio

#endif
