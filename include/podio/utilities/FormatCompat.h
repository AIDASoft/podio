#ifndef PODIO_UTILITIES_FORMATCOMPAT_H
#define PODIO_UTILITIES_FORMATCOMPAT_H

// fmt/core.h is enough for all the error reporting functionality below and it
// also provides FMT_VERSION
#include <fmt/core.h>

namespace podio::detail {

/// Report a format error at compile time or, via a fmt::format_error exception,
/// at runtime.
///
/// The function that fmt provides for this purpose has changed over the
/// different versions:
/// - fmt 9 and 10: fmt::detail::throw_format_error (fmt 10 additionally exposes
///   it as fmt::throw_format_error)
/// - fmt 11 and newer: fmt::report_error (fmt::throw_format_error is deprecated
///   in fmt 11.0 and removed afterwards)
///
/// This is intentionally not constexpr to give a compile time error when a
/// format string is checked at compile time.
[[noreturn]] inline void reportFormatError(const char* message) {
#if FMT_VERSION >= 110000
  fmt::report_error(message);
#else
  fmt::detail::throw_format_error(message);
#endif
}

} // namespace podio::detail

#endif // PODIO_UTILITIES_FORMATCOMPAT_H
