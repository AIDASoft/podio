#ifndef PODIO_GLOB_UTILS_H // NOLINT(llvm-header-guard): internal headers confuse clang-tidy
#define PODIO_GLOB_UTILS_H // NOLINT(llvm-header-guard): internal headers confuse clang-tidy
#include <string>
#include <vector>

#if __has_include(<glob.h>)
  #include <glob.h>
#endif

namespace podio::detail {
std::vector<std::string> expand_glob(const std::string& pattern);
}

#if __has_include(<glob.h>)
std::vector<std::string> podio::detail::expand_glob(const std::string& pattern) {
  glob_t glob_result;
  if (glob(pattern.c_str(), GLOB_TILDE | GLOB_BRACE, nullptr, &glob_result) != 0) {
    globfree(&glob_result);
    return {pattern};
  }
  std::vector<std::string> results;
  results.reserve(glob_result.gl_pathc);
  for (size_t i = 0; i < glob_result.gl_pathc; ++i) {
    results.emplace_back(glob_result.gl_pathv[i]);
  }
  globfree(&glob_result);
  return results;
}
#else
std::vector<std::string> podio::detail::expand_glob(const std::string& pattern) {
  return {pattern};
}
#endif //  __has_include(<glob.h>)
#endif // PODIO_GLOB_UTILS_H
