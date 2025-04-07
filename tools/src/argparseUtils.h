#ifndef PODIO_TOOLS_ARGPARSEUTILS_H // NOLINT(llvm-header-guard): folder structure not suitable
#define PODIO_TOOLS_ARGPARSEUTILS_H // NOLINT(llvm-header-guard): folder structure not suitable

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

template <typename... Flags>
auto findFlags(const std::vector<std::string>& argv, Flags... flags) {
  return std::find_if(argv.begin(), argv.end(), [&](const auto& elem) { return ((elem == flags) || ...); });
}

inline std::vector<std::string> splitString(const std::string& str, const char delimiter) {
  std::vector<std::string> tokens;
  std::string token;
  for (char ch : str) {
    if (ch == delimiter) {
      if (!token.empty()) {
        tokens.push_back(token);
        token.clear();
      }
    } else {
      token += ch;
    }
  }
  if (!token.empty()) {
    tokens.push_back(token);
  }
  return tokens;
}

inline size_t parseSizeOrExit(const std::string& str) {
  try {
    std::size_t pos{};
    const auto number = std::stoull(str, &pos);
    if (pos != str.size()) {
      throw std::invalid_argument("");
    }
    return number;
  } catch (const std::invalid_argument&) {
    std::cerr << "'" << str << "' cannot be parsed into an integer number" << std::endl;
    std::exit(1);
  }
}

#endif // PODIO_TOOLS_ARGPARSEUTILS_H
