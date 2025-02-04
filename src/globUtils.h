#ifndef PODIO_GLOB_UTILS_H // NOLINT(llvm-header-guard): internal headers confuse clang-tidy
#define PODIO_GLOB_UTILS_H // NOLINT(llvm-header-guard): internal headers confuse clang-tidy
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#if __has_include(<glob.h>)
  #include <glob.h>
#else
  #include <system_error>
#endif

namespace podio::detail {
/**
 * @brief Expands a given glob pattern into a list of matching file paths.
 *
 * This function takes a glob pattern as input and returns a vector of strings
 * containing the paths that match the pattern. It supports standard glob rules
 * extended with tilde expansion and brace expansion If the pattern doesn't
 * contain any wildcards then it is placed in the returned vector as is. Paths
 * that cannot be accessed are displayed on std::cerr, but the expansion process
 * is not aborted. On platforms without <glob.h> no expansion is done and vector
 * containing the original pattern is returned
 *
 * @param pattern The glob pattern to expand.
 * @return A vector of strings containing the matching file paths.
 *
 * @throws std::runtime_error If no matches are found or if there is an error
 *         during glob expansion.
 */
std::vector<std::string> expand_glob(const std::string& pattern);
/**
 * @brief Checks if a given pattern is a glob pattern.
 *
 * This function determines whether the provided pattern contains any standard
 * glob or brace expansion wildcards.
 *
 * @param pattern The pattern to check.
 * @return true if the pattern is a glob pattern, false otherwise.
 */
bool is_glob_pattern(const std::string& pattern);
} // namespace podio::detail

bool podio::detail::is_glob_pattern(const std::string& pattern) {
  bool escape = false;
  for (auto c : pattern) {
    if (escape) {
      escape = false;
    } else if (c == '\\') {
      escape = true;
    } else if (c == '*' || c == '?' || c == '[' || c == '{') {
      return true;
    }
  }
  return false;
}

#if __has_include(<glob.h>)

int glob_err_handler(const char* epath, int eerrno) {
  std::cerr << "Glob expansion error accessing path: " << epath << " (error code: " << eerrno << ")\n";
  return 0;
}

std::vector<std::string> podio::detail::expand_glob(const std::string& pattern) {
  glob_t glob_result;
  auto retv = glob(pattern.c_str(), GLOB_TILDE | GLOB_BRACE | GLOB_NOMAGIC, glob_err_handler, &glob_result);
  if (retv == GLOB_NOMATCH) {
    throw std::runtime_error("Glob expansion found no matches for pattern: " + pattern);
  } else if (retv != 0) {
    globfree(&glob_result);
    throw std::runtime_error("Glob expansion error");
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
  if (is_glob_pattern(pattern)) {
    throw std::system_error("Glob expansion is not supported on this platform")
  }
  return {pattern};
}

#endif //  __has_include(<glob.h>)

#endif // PODIO_GLOB_UTILS_H
