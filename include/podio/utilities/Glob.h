#ifndef PODIO_UTILITIES_GLOB_H
#define PODIO_UTILITIES_GLOB_H
#include <string>
#include <vector>

namespace podio::utils {
/**
 * @brief Expands a given glob pattern into a list of matching file paths.
 *
 * This function takes a glob pattern as input and returns a vector of strings
 * containing the paths that match the pattern. It supports standard glob rules
 * extended with tilde expansion and brace expansion. If the pattern doesn't
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

} // namespace podio::utils

#endif // PODIO_UTILITIES_GLOB_H
