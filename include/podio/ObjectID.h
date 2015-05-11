#ifndef OBJECTID_H
#define OBJECTID_H

namespace podio {

class ObjectID {

public:
  int index; /// index in collection
  int collectionID; /// ID if the collection

  static const int untracked = -1; /// not part of a collection
  static const int invalid   = -2; /// invalid or non-available object
  //static const int transient = -3;

};

} // namespace

#endif
