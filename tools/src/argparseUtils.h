#ifndef PODIO_TOOLS_ARGPARSEUTILS_H // NOLINT(llvm-header-guard): folder structure not suitable
#define PODIO_TOOLS_ARGPARSEUTILS_H // NOLINT(llvm-header-guard): folder structure not suitable

#include <algorithm>
#include <charconv>
#include <iostream>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

template <typename... Flags>
auto findFlags(const std::vector<std::string>& argv, Flags... flags) {
  return std::find_if(argv.begin(), argv.end(), [&](const auto& elem) { return ((elem == flags) || ...); });
}

inline std::vector<std::string_view> splitString(const std::string& str, const char delimiter) {
  std::vector<std::string_view> tokens;

  for (const auto& token : std::ranges::views::split(str, delimiter)) {
    tokens.emplace_back(token.begin(), token.end());
  }

  return tokens;
}

inline size_t parseSizeOrExit(std::string_view str) {
  try {
    std::size_t number{};
    const auto [ptr, err] = std::from_chars(str.data(), str.data() + str.size(), number);
    if (err != std::errc{} || ptr != str.data() + str.size()) {
      throw std::invalid_argument("");
    }
    return number;
  } catch (const std::invalid_argument&) {
    std::cerr << "'" << str << "' cannot be parsed into an integer number" << std::endl;
    std::exit(1);
  }
}

#endif // PODIO_TOOLS_ARGPARSEUTILS_H
