#ifndef PODIO_UTILITIES_MISCHELPERS_H
#define PODIO_UTILITIES_MISCHELPERS_H

#include <algorithm>
#include <string>
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
} // namespace podio::utils

#endif // PODIO_UTILITIES_MISCHELPERS_H
