#ifndef PODIO_TOOLS_ARGPARSEUTILS_H // NOLINT(llvm-header-guard): folder structure not suitable
#define PODIO_TOOLS_ARGPARSEUTILS_H // NOLINT(llvm-header-guard): folder structure not suitable

#include <fmt/core.h>

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

void printUsageAndExit(const char* usageMsg) {
  fmt::println(stderr, "{}", usageMsg);
  std::exit(1);
}

auto getArgumentValueOrExit(const std::vector<std::string>& argv, std::vector<std::string>::const_iterator it,
                            const char* usageMsg) {
  const int argc = argv.size();
  const auto index = std::distance(argv.begin(), it);
  if (index > argc - 2) {
    printUsageAndExit(usageMsg);
  }
  return argv[index + 1];
}

/// Check if -h or --help is in the flags and exit in case it is after printing
/// the help message
void printHelpAndExit(const std::vector<std::string>& argv, const char* usageMsg, const char* helpMsg) {
  if (const auto it = findFlags(argv, "-h", "--help"); it != argv.end()) {
    fmt::print("{}\n{}", usageMsg, helpMsg);
    std::exit(0);
  }
}

#endif // PODIO_TOOLS_ARGPARSEUTILS_H
