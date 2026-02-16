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

/// CRTP base for fmt::formatters that support ADL-based custom formatting.
///
/// Provides the common parse/format logic shared by all podio formatters that
/// support the 'u' (user-defined via ADL) format specifier. The default format
/// specifier is 'd' (detailed). Additional specifiers (e.g. 'b' for brief) can
/// be added via the ExtraSpecifiers template parameter pack.
///
/// @tparam T               The type being formatted
/// @tparam Derived         The concrete fmt::formatter specialization (CRTP)
/// @tparam ExtraSpecifiers Additional single-char format specifiers beyond 'd' and 'u'
///
/// Derived classes must implement:
///   fmt::format_context::iterator formatDefault(const T& value, fmt::format_context& ctx) const;
template <typename T, typename Derived, char... ExtraSpecifiers>
struct ADLFormatter {
  char presentation = 'd';

  constexpr auto parse(fmt::format_parse_context& ctx) {
    auto it = ctx.begin();
    auto end = ctx.end();
    if (it != end && *it != '}') {
      presentation = *it++;
      if (presentation != 'd' && presentation != 'u' && ((presentation != ExtraSpecifiers) && ...)) {
        fmt::throw_format_error("Invalid format specifier");
      }
      if (presentation == 'u') {
        detail::requireCustomFormat<T>();
      }
    }
    if (it != end && *it != '}') {
      fmt::throw_format_error("Invalid format specifier");
    }
    return it;
  }

  fmt::format_context::iterator format(const T& value, fmt::format_context& ctx) const {
    if (presentation == 'u') {
      return detail::dispatchCustomFormat(value, ctx);
    }
    return static_cast<const Derived&>(*this).formatDefault(value, ctx);
  }
};

} // namespace podio

#endif // PODIO_UTILITIES_FORMATHELPERS_H
