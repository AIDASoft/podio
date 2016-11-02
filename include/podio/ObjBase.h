#ifndef OBJBASE_H
#define OBJBASE_H

#include <atomic>
#include <iostream>
#include <stdexcept>
#include "podio/ObjectID.h"

namespace podio {

  class ObjBase {
  public:

    /// Constructor from ObjectID and initial object-count
    ObjBase(ObjectID id, int i) : id(id) , ref_counter(i) {};

    /// checks whether object is "untracked" by a collection
    /// if yes, increases reference count
    void acquire() {
      if (id.index == podio::ObjectID::untracked) ++ref_counter;
    };

    /// checks whether object is "untracked" by a collection
    /// if yes, decrease reference count and delete itself if count===0
    int release(){
      if (id.index != podio::ObjectID::untracked){ return 1;};
      if (--ref_counter == 0) {
        delete this;
      }
      return 0;
    };

    void setReadOnly(){ is_readonly = true;}
    void checkAccess() throw( std::runtime_error ) {if (is_readonly) throw std::runtime_error("error");}

    /// destructor
    virtual ~ObjBase(){};

  public:
    /// ID of the object
    podio::ObjectID id;

  private:
    /// whether the object is read-only
    bool is_readonly{false};
    /// reference counter
    std::atomic<int> ref_counter;

  };

} // namespace

#endif
