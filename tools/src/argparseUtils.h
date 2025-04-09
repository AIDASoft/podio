#ifndef PODIO_TOOLS_ARGPARSEUTILS_H // NOLINT(llvm-header-guard): folder structure not suitable
#define PODIO_TOOLS_ARGPARSEUTILS_H // NOLINT(llvm-header-guard): folder structure not suitable

#include <algorithm>
#include <charconv>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

template <typename... Flags>
auto findFlags(const std::vector<std::string>& argv, Flags... flags) {
  return std::find_if(argv.begin(), argv.end(), [&](const auto& elem) { return ((elem == flags) || ...); });
}

inline size_t parseSizeOrExit(std::string_view str) {
  std::size_t number{};
  const auto [ptr, err] = std::from_chars(str.data(), str.end(), number);
  if (err != std::errc{} || ptr != str.end()) {
    std::cerr << "'" << str << "' cannot be parsed into an integer number" << std::endl;
    std::exit(1);
  }
  return number;
}

#endif // PODIO_TOOLS_ARGPARSEUTILS_H
