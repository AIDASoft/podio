#ifndef PODIO_UTILITIES_MISCHELPERS_H
#define PODIO_UTILITIES_MISCHELPERS_H

#include <algorithm>
#include <cctype>
#include <compare>
#include <iostream>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace podio::utils {

///  Sort the input vector of strings alphabetically, case insensitive.
///  Names that differ only by case are ordered by their case-sensitive spelling.
///
///  @param strings The strings that should be sorted alphabetically
///
///  @returns A vector of strings sorted alphabetically, case insensitive
inline std::vector<std::string> sortAlphabeticaly(std::vector<std::string> strings) {
  std::ranges::sort(strings, [](const auto& lhs, const auto& rhs) {
    const auto order = std::lexicographical_compare_three_way(
        lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
        [](unsigned char cl, unsigned char cr) { return std::tolower(cl) <=> std::tolower(cr); });
    return order == 0 ? lhs < rhs : order < 0;
  });
  return strings;
}

/// Sort the input vector of strings alphabetically and remove duplicates.
/// Prints a warning if duplicates are found.
///
/// @param strings The strings that should be sorted and deduplicated
/// @param context A context string for the warning message (e.g., "collsToRead", "collsToWrite")
///
/// @returns A vector of unique strings sorted alphabetically, case insensitive
inline std::vector<std::string> sortAndDeduplicate(std::vector<std::string> strings, std::string_view context = "") {
  const auto originalSize = strings.size();
  strings = sortAlphabeticaly(std::move(strings));

  const auto uniqueResult = std::ranges::unique(strings);
  strings.erase(uniqueResult.begin(), uniqueResult.end());

  if (strings.size() < originalSize) {
    std::cerr << "WARNING: Found " << (originalSize - strings.size()) << " duplicate collection(s)";
    if (!context.empty()) {
      std::cerr << " in " << context;
    }
    std::cerr << ". Duplicates have been removed." << std::endl;
  }

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
