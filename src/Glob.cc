#include "podio/utilities/Glob.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#if __has_include(<glob.h>)
  #include <glob.h>
#else
  #include <system_error>
#endif

namespace podio::utils {

bool is_glob_pattern(const std::string& pattern) {
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

std::vector<std::string> expand_glob(const std::string& pattern) {
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

std::vector<std::string> expand_glob(const std::string& pattern) {
  if (is_glob_pattern(pattern)) {
    throw std::system_error("Glob expansion is not supported on this platform")
  }
  return {pattern};
}

#endif //  __has_include(<glob.h>)

} // namespace podio::utils
