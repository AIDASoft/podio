#ifndef PODIO_ASSOCIATIONOBJ_H
#define PODIO_ASSOCIATIONOBJ_H

#include "podio/detail/AssociationFwd.h"

#include "podio/ObjBase.h"
#include "podio/ObjectID.h"

namespace podio {

template <typename FromT, typename ToT>
class AssociationObj : public podio::ObjBase {

  friend Association<FromT, ToT>;
  friend MutableAssociation<FromT, ToT>;

public:
  /// Constructor
  AssociationObj() :
      ObjBase{{podio::ObjectID::untracked, podio::ObjectID::untracked}, 0},
      weight(0.0f),
      m_from(nullptr),
      m_to(nullptr) {
  }

  /// Constructor from ObjectID and weight (does not initialize relations yet!)
  AssociationObj(const podio::ObjectID id_, float weight_) : ObjBase{id_, 0}, weight(weight_) {
  }

  /// Copy constructor (deep-copy of relations)
  AssociationObj(const AssociationObj& other) :
      ObjBase{{podio::ObjectID::untracked, podio::ObjectID::untracked}, 0},
      weight(other.weight),
      m_from(nullptr),
      m_to(nullptr) {
    if (other.m_from) {
      m_from = new FromT(*other.m_from);
    }
    if (other.m_to) {
      m_to = new ToT(*other.m_to);
    }
  }

  /// No assignment operator
  AssociationObj& operator=(const AssociationObj&) = delete;

  /// Destructor
  ~AssociationObj() {
    delete m_from;
    delete m_to;
  }

public:
  float weight{};
  FromT* m_from{nullptr};
  ToT* m_to{nullptr};
};

} // namespace podio

#endif // PODIO_ASSOCIATIONOBJ_H
