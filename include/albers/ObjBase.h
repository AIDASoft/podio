#ifndef OBJBASE_H
#define OBJBASE_H

#include <atomic>
#include <iostream>
#include "albers/ObjectID.h"

namespace albers {

  class ObjBase {
  public:

    /// Constructor from ObjectID and initial object-count
    ObjBase(ObjectID id, int i) : id(id) , ref_counter(i) {};

    /// checks whether object is "untracked" by a collection
    /// if yes, increases reference count
    void acquire() {
      if (id.index == albers::ObjectID::untracked) ++ref_counter;
    };

    /// checks whether object is "untracked" by a collection
    /// if yes, decrease reference count and delete itself if count===0
    int release(){
      if (id.index != albers::ObjectID::untracked){ return 1;};
      if (--ref_counter == 0) {
        //std::cout << "deleting free-floating object at " << this << std::endl;
        delete this;
      }
      return 0;
    };

    /// destructor
    virtual ~ObjBase(){};

  public:
    /// ID of the object
    albers::ObjectID id;

  private:
    /// reference counter
    std::atomic<int> ref_counter;

  };

} // namespace

#endif
