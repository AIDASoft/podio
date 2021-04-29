#ifndef PODIO_OBJBASE_H
#define PODIO_OBJBASE_H

#include "podio/ObjectID.h"
#include <atomic>
#include <iostream>

namespace podio {

class ObjBase {
public:
  /// Constructor from ObjectID and initial object-count
  ObjBase(ObjectID id_, unsigned i) : id(id_), ref_counter(i){};

  /// checks whether object is "untracked" by a collection
  /// if yes, increases reference count
  void acquire() {
    if (id.index == podio::ObjectID::untracked) {
      ++ref_counter;
    }
  };

  /// checks whether object is "untracked" by a collection
  /// if yes, decrease reference count and delete itself if count===0
  int release() {
    if (id.index != podio::ObjectID::untracked) {
      return 1;
    };
    if (--ref_counter == 0) {
      delete this;
    }
    return 0;
  };

  /// destructor
  virtual ~ObjBase() = default;

public:
  /// ID of the object
  podio::ObjectID id;

private:
  /// reference counter
  std::atomic<unsigned> ref_counter;
};

} // namespace podio

#endif
