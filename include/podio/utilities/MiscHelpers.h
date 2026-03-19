#ifndef PODIO_UTILITIES_MISCHELPERS_H
#define PODIO_UTILITIES_MISCHELPERS_H

#include <algorithm>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

namespace podio::utils {

///  Sort the input vector of strings alphabetically, case insensitive.
///
///  @param strings The strings that should be sorted alphabetically
///
///  @returns A vector of strings sorted alphabetically, case insensitive
inline std::vector<std::string> sortAlphabeticaly(std::vector<std::string> strings) {
  // Obviously there is no tolower(std::string) in c++, so this is slightly more
  // involved and we make use of the fact that lexicographical_compare works on
  // ranges and the fact that we can feed it a dedicated comparison function,
  // where we convert the strings to lower case char-by-char. The alternative is
  // to make string copies inside the first lambda, transform them to lowercase
  // and then use operator< of std::string, which would be effectively
  // hand-writing what is happening below.
  std::ranges::sort(strings, [](const auto& lhs, const auto& rhs) {
    return std::lexicographical_compare(
        lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
        [](const auto& cl, const auto& cr) { return std::tolower(cl) < std::tolower(cr); });
  });
  return strings;
}

/// Split a string (view) at the delimiter and return a range of views
///
/// @param str The string to split
/// @param delim The delimeter at which to split
///
/// @returns A range of views into the original view
inline auto splitString(const std::string_view str, const char delim) {
  namespace rv = std::ranges::views;

  return str | rv::split(delim) |
#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 12
      // gcc 11 is missing a string_view constructor that takes the range
      // iterators directly, so we construct from a char* and a size
      rv::transform(
             [](auto&& subrange) { return std::string_view(&*subrange.begin(), std::ranges::distance(subrange)); });
#else
      rv::transform([](auto&& subrange) { return std::string_view(subrange.begin(), subrange.end()); });
#endif
}

} // namespace podio::utils

#endif // PODIO_UTILITIES_MISCHELPERS_H
