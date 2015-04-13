#ifndef OBJECTID_H
#define OBJECTID_H

namespace albers {

class ObjectID {

public:
  int index;
  int collectionID;

  static const int untracked = -1;
  static const int invalid   = -2;
  //static const int transient = -3;

};

} // namespace

#endif
