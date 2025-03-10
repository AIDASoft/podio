#ifndef PODIO_UTILITIES_STATICCONCATENATE_H
#define PODIO_UTILITIES_STATICCONCATENATE_H

#include <algorithm>
#include <array>
#include <string_view>
namespace podio::utils {

/// Helper struct to concatenate a set of string_views into a single string_view at compile time
template <const std::string_view&... strs>
struct static_concatenate {
  static constexpr auto init_arr() {
    constexpr auto total_size = (strs.size() + ... + 1); // reserve space for '\0'
    auto arr = std::array<char, total_size>();
    auto it = arr.begin();
    ((it = std::ranges::copy(strs, it).out), ...);
    arr.back() = '\0';
    return arr;
  }
  constexpr static auto array = init_arr();
  constexpr static auto value = std::string_view(array.data(), array.size() - 1); // skip '\0'
};

/// Variable template for concatenating a set of string_views into a single string_view at compile time
template <const std::string_view&... strs>
inline constexpr std::string_view static_concatenate_v = static_concatenate<strs...>::value;

} // namespace podio::utils
#endif // PODIO_UTILITIES_STATICCONCATENATE_H
