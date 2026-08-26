#ifndef PODIO_UTILITIES_FORMATHELPERS_H
#define PODIO_UTILITIES_FORMATHELPERS_H

#include "podio/utilities/FormatCompat.h"

#include <fmt/format.h>

#include <algorithm>
#include <array>
#include <string_view>

namespace podio {

namespace detail {
  /// Concept to detect if customPodioFormat is defined for type T. Uses
  /// unqualified lookup so that ADL can find overloads defined in the same
  /// namespace as the type. Users should define their customPodioFormat
  /// overloads in the same namespace as their type (following the same pattern
  /// as std::swap).
  template <typename T>
  concept HasCustomFormat = requires(const T& val, fmt::format_context& ctx) {
    { customPodioFormat(val, ctx) } -> std::same_as<fmt::format_context::iterator>;
  };

  /// Dispatch helper: calls customPodioFormat if available, otherwise throws a format
  /// error.
  template <typename T>
  fmt::format_context::iterator dispatchCustomFormat(const T& val, fmt::format_context& ctx) {
    if constexpr (HasCustomFormat<T>) {
      return customPodioFormat(val, ctx);
    } else {
      podio::detail::reportFormatError("Format specifier 'u' requires a customPodioFormat for this type");
      return ctx.out(); // unreachable, silences warnings
    }
  }

  /// Build a compile-time error message listing supported format specifiers.
  /// Produces a message like: "Invalid format specifier. Supported: 'b', 'd', 'u'"
  template <char... Specs>
  consteval auto buildSpecErrorMessage() {
    constexpr std::size_t N = sizeof...(Specs);
    constexpr std::string_view prefix = "Invalid format specifier. Supported: ";
    constexpr std::string_view delim = ", ";

    // result buffer size = prefix + 3 chars per spec and appropriate number of
    // delimiters and null terminator
    constexpr auto bufLen = prefix.size() + (N * 3) + (N > 0 ? (N - 1) * delim.size() : 0) + 1;
    std::array<char, bufLen> result{};
    auto out = result.begin();
    out = std::ranges::copy(prefix, out).out;

    if constexpr (N == 0) {
      return result;
    }

    // Sort specifiers alphabetically for consistent display
    std::array<char, N> specs{Specs...};
    std::ranges::sort(specs);

    auto format_spec = [&](char c) {
      const std::array wrapped = {'\'', c, '\''};
      out = std::ranges::copy(wrapped, out).out;
    };
    // First spec without leading delimiter
    format_spec(specs[0]);
    // The rest with leading delimieter
    std::ranges::for_each(std::ranges::subrange(specs.begin() + 1, specs.end()), [&](char c) {
      out = std::ranges::copy(delim, out).out;
      format_spec(c);
    });

    return result;
  }

  template <char... Specs>
  inline constexpr auto specErrorMsg = buildSpecErrorMessage<Specs...>();

} // namespace detail

/// CRTP base for fmt::formatters that support ADL-based custom formatting.
///
/// Provides the common parse/format logic shared by all podio formatters that
/// support the 'u' (user-defined via ADL) format specifier. The default format
/// specifier is 'd' (default - tries user-defined, falls back to code-generated).
/// The 'g' specifier always uses code-generated formatting. Additional specifiers
/// (e.g. 'b' for brief) can be added via the ExtraSpecifiers template parameter pack.
///
/// @tparam T               The type being formatted
/// @tparam Derived         The concrete fmt::formatter specialization (CRTP)
/// @tparam ExtraSpecifiers Additional single-char format specifiers beyond 'd', 'g', and 'u'
///
/// Derived classes must implement:
///   fmt::format_context::iterator formatImpl(const T& value, fmt::format_context& ctx) const;
template <typename T, typename Derived, char... ExtraSpecifiers>
struct ADLFormatter {
  char presentation = 'd';

  constexpr auto parse(fmt::format_parse_context& ctx) {
    auto it = ctx.begin();
    auto end = ctx.end();
    if (it != end && *it != '}') {
      presentation = *it++;
      bool valid = (it == end) || (*it == '}');
      // First check if we have a custom format available and can use it
      if (presentation == 'u') {
        if (detail::HasCustomFormat<T> && valid) {
          return it;
        }
        podio::detail::reportFormatError("Format specifier 'u' requires an overload of defineCustomPodioFormat for this type");
      }

      // Now check the rest and emit a corresponding error message depending on
      // whether 'u' is available or not
      if (valid && presentation != 'd' && presentation != 'g' && ((presentation != ExtraSpecifiers) && ...)) {
        if constexpr (detail::HasCustomFormat<T>) {
          podio::detail::reportFormatError(detail::specErrorMsg<'d', 'g', 'u', ExtraSpecifiers...>.data());
        } else {
          podio::detail::reportFormatError(detail::specErrorMsg<'d', 'g', ExtraSpecifiers...>.data());
        }
      }
    }
    return it;
  }

  fmt::format_context::iterator format(const T& value, fmt::format_context& ctx) const {
    if (presentation == 'u') {
      return detail::dispatchCustomFormat(value, ctx);
    }
    if (presentation == 'd') {
      // For 'd', try user-defined formatting if available, otherwise fall back to code-generated
      if constexpr (detail::HasCustomFormat<T>) {
        return detail::dispatchCustomFormat(value, ctx);
      } else {
        return static_cast<const Derived&>(*this).formatImpl(value, ctx);
      }
    }
    // For 'g' and any ExtraSpecifiers, always use code-generated formatting
    return static_cast<const Derived&>(*this).formatImpl(value, ctx);
  }
};

} // namespace podio

#endif // PODIO_UTILITIES_FORMATHELPERS_H
