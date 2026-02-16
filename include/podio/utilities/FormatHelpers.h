#ifndef PODIO_UTILITIES_FORMATHELPERS_H
#define PODIO_UTILITIES_FORMATHELPERS_H

#include <fmt/format.h>

namespace podio {

/// Concept to detect if customFormat is defined for type T.
/// Uses unqualified lookup so that ADL can find overloads defined in the same
/// namespace as the type. Users should define their customFormat overloads in
/// the same namespace as their type (following the same pattern as std::swap).
template <typename T>
concept HasCustomFormat = requires(const T& val, fmt::format_context& ctx) {
  { customFormat(val, ctx) } -> std::same_as<fmt::format_context::iterator>;
};

namespace detail {
  /// Dispatch helper: calls customFormat if available, otherwise throws a format
  /// error. The requireCustomFormat check in parse() catches this earlier (at
  /// compile time for compile-time format strings), but this serves as a safety
  /// net for runtime format strings.
  template <typename T>
  fmt::format_context::iterator dispatchCustomFormat(const T& val, fmt::format_context& ctx) {
    if constexpr (HasCustomFormat<T>) {
      return customFormat(val, ctx);
    } else {
      fmt::throw_format_error("Format specifier 'u' requires a podio::customFormat overload for this type");
      return ctx.out(); // unreachable, silences warnings
    }
  }

  /// Call from a constexpr parse() method to reject the 'u' format specifier
  /// for types that don't provide a customFormat overload. With compile-time
  /// format strings (the default for fmt::format), this produces a compile-time
  /// error. With fmt::runtime() format strings it produces a runtime error.
  template <typename T>
  constexpr void requireCustomFormat() {
    if constexpr (!HasCustomFormat<T>) {
      fmt::throw_format_error("Format specifier 'u' requires a podio::customFormat overload for this type");
    }
  }
} // namespace detail
} // namespace podio

#endif // PODIO_UTILITIES_FORMATHELPERS_H
