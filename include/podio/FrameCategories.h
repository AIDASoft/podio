#ifndef PODIO_FRAMECATEGORIES_H
#define PODIO_FRAMECATEGORIES_H

#include <string>

namespace podio {

/// Create a parameterName that encodes the collection name and the parameter
/// Name into one string.
///
/// This codifies a convention that was decided on to store collection level
/// parameters. These are parameters / metadata that are valid for all
/// collections of a given name in a file, e.g. CellID encoding strings. These
/// parameters are usually stored in a dedicated metadata Frame inside a file,
/// see the predefined category names in the Category namespace.
///
/// @param collName the name of the collection
/// @param paramName the name of the parameter
///
/// @returns A single key string that combines the collection and parameter name
inline std::string collMetadataParamName(const std::string& collName, const std::string& paramName) {
  return collName + "__" + paramName;
}

/// This namespace mimics an enum (at least in its usage) and simply defines
/// either commonly used category names, or category names that form a
/// convention.
namespace Category {
  /// The event category
  constexpr const auto Event = "events";
  /// The run category
  constexpr const auto Run = "runs";
  /// The metadata category that is used to store a single Frame that holds data
  /// that is valid for a whole file, for example collection level parameters
  constexpr const auto Metadata = "metadata";
} // namespace Category
} // namespace podio

#endif
