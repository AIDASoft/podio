#ifndef PODIO_OBJECTID_H
#define PODIO_OBJECTID_H

#include <fmt/core.h>

#include <cstdint>
#include <functional>
#include <iterator>
#include <ostream>

#if defined(PODIO_JSON_OUTPUT) && !defined(__CLING__)
  #include "nlohmann/json.hpp"
#endif

namespace podio {

class ObjectID {

public:
  /// not part of a collection
  static const int untracked = -1;
  /// invalid or non-available object
  static const int invalid = -2;

  /// index of object in collection
  int index{untracked};
  /// ID of the collection
  uint32_t collectionID{static_cast<uint32_t>(untracked)};

  /// index and collectionID uniquely defines the object.
  /// this operator is necessary for meaningful comparisons in python
  bool operator==(const ObjectID& other) const {
    return index == other.index && collectionID == other.collectionID;
  }
  bool operator!=(const ObjectID& other) const {
    return !(*this == other);
  }
};

#if defined(PODIO_JSON_OUTPUT) && !defined(__CLING__)
inline void to_json(nlohmann::json& j, const podio::ObjectID& id) {
  j = nlohmann::json{{"collectionID", id.collectionID}, {"index", id.index}};
}
#endif

} // namespace podio

template <>
struct std::hash<podio::ObjectID> {
  std::size_t operator()(const podio::ObjectID& id) const noexcept {
    auto hash_collectionID = std::hash<uint32_t>{}(id.collectionID);
    auto hash_index = std::hash<int>{}(id.index);

    return hash_collectionID ^ hash_index;
  }
};

template <>
struct fmt::formatter<podio::ObjectID> {
  constexpr auto parse(fmt::format_parse_context& ctx) {
    auto it = ctx.begin();
    if (it != ctx.end() && *it != '}') {
      fmt::throw_format_error("Invalid format. ObjectId does not support specifiers");
    }
    return it;
  }

  auto format(const podio::ObjectID& obj, fmt::format_context& ctx) const {
    return fmt::format_to(ctx.out(), "{:8x}|{}", obj.collectionID, obj.index);
  }
};

namespace podio {
inline std::ostream& operator<<(std::ostream& os, const podio::ObjectID& id) {
  fmt::format_to(std::ostreambuf_iterator<char>(os), "{}", id);
  return os;
}
} // namespace podio

#endif
