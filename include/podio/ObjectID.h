#ifndef OBJECTID_H
#define OBJECTID_H

namespace podio {

class ObjectID {

public:
   /// index of object in collection
  int index;
  /// ID of the collection
  int collectionID;

  /// not part of a collection
  static const int untracked = -1;
  /// invalid or non-available object
  static const int invalid   = -2;

  /// index and collectionID uniquely defines the object.
  /// this operator is necessary for meaningful comparisons in python
  bool operator==(const ObjectID& other) const {return index == other.index &&
						collectionID == other.collectionID; }
};

} // namespace

#endif
