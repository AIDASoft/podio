#ifndef PODIO_DETAIL_LINKOBJ_H
#define PODIO_DETAIL_LINKOBJ_H

#include "podio/detail/LinkFwd.h"

#include "podio/ObjectID.h"

namespace podio {

template <typename FromT, typename ToT>
class LinkObj {

  friend Link<FromT, ToT>;
  friend MutableLink<FromT, ToT>;

public:
  /// Constructor
  LinkObj() : id(), weight(0.0f), m_from(nullptr), m_to(nullptr) {
  }

  /// Constructor from ObjectID and weight (does not initialize relations yet!)
  LinkObj(const podio::ObjectID id_, float weight_) : id(id_), weight(weight_) {
  }

  /// Copy constructor (deep-copy of relations)
  LinkObj(const LinkObj& other) : id(), weight(other.weight), m_from(nullptr), m_to(nullptr) {
    if (other.m_from) {
      m_from = new FromT(*other.m_from);
    }
    if (other.m_to) {
      m_to = new ToT(*other.m_to);
    }
  }

  /// No assignment operator
  LinkObj& operator=(const LinkObj&) = delete;

  /// Destructor
  ~LinkObj() {
    delete m_from;
    delete m_to;
  }

public:
  podio::ObjectID id{};
  float weight{1.0f};
  FromT* m_from{nullptr};
  ToT* m_to{nullptr};
};

} // namespace podio

#endif // PODIO_DETAIL_LINKOBJ_H
