#ifndef PODIO_OBJECTID_H
#define PODIO_OBJECTID_H

#include <cstdint>
#include <iomanip>
#include <ostream>

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
};

inline std::ostream& operator<<(std::ostream& os, const podio::ObjectID& id) {
  const auto oldFlags = os.flags();
  os << std::hex << std::setw(8) << id.collectionID;
  os.flags(oldFlags);
  return os << id.index;
}

} // namespace podio

#endif
