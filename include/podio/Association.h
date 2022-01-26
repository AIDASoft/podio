#ifndef PODIO_ASSOCIATION_H
#define PODIO_ASSOCIATION_H

#include "podio/AssociationObj.h"

#include <ostream>
#include <utility> // std::swap

namespace podio {

/**
 * Generalized Association type for both Mutable and immutable (default)
 * versions. User facing clases with the expected naming scheme are defined via
 * template aliases are defined just below
 */
template <typename FromT, typename ToT, bool Mutable>
class AssociationT {
public:
  /// Constructor
  AssociationT() : m_obj(new AssociationObj<FromT, ToT>()) {
    m_obj->acquire();
  }

  /// Constructor with weight
  AssociationT(float weight) : m_obj(new AssociationObj<FromT, ToT>()) {
    m_obj->acquire();
    m_obj->weight = weight;
  }

  /// Constructor from existing AssociationObj
  AssociationT(AssociationObj<FromT, ToT>* obj) : m_obj(obj) {
    if (m_obj) {
      m_obj->acquire();
    }
  }

  /// Copy constructor
  AssociationT(const AssociationT& other) : m_obj(other.m_obj) {
    if (m_obj) {
      m_obj->acquire();
    }
  }

  /// Assignment operator
  AssociationT& operator=(AssociationT other) {
    swap(*this, other);
    return *this;
  }

  /// Destructor
  ~AssociationT() {
    if (m_obj) {
      m_obj->release();
    }
  }

  float getWeight() const {
    return m_obj->weight;
  }

  /// Access the related-from object
  const FromT getFrom() const {
    if (!m_obj->m_from) {
      return FromT(nullptr);
    }
    return FromT(m_obj->m_from);
  }

  /// Access the related-to object
  const ToT getTo() const {
    if (!m_obj->m_to) {
      return FromT(nullptr);
    }
    return FromT(m_obj->m_to);
  }

  /// check whether the object is actually available
  bool isAvailable() const {
    return m_obj;
  }

  /// disconnect from Association instance
  void unlink() {
    m_obj = nullptr;
  }

  /// Get the ObjectID
  podio::ObjectID getObjectID() const {
    if (m_obj) {
      return m_obj->id;
    }
    return podio::ObjectID{podio::ObjectID::invalid, podio::ObjectID::invalid};
  }

  unsigned int id() const {
    return getObjectID().collectionID * 10000000 + getObjectID().index;
  }

  bool operator==(const AssociationT& other) const {
    return m_obj == other.m_obj;
  }

  bool operator<(const AssociationT& other) const {
    return m_obj < other.m_obj;
  }

  friend void swap(AssociationT& a, AssociationT& b) {
    using std::swap;
    swap(a.m_obj, b.m_obj); // swap out the internal pointers
  }

private:
  AssociationObj<FromT, ToT>* m_obj{nullptr};
}; // namespace podio

template <typename FromT, typename ToT>
using Association = AssociationT<FromT, ToT, false>;

template <typename FromT, typename ToT>
using MutableAssociation = AssociationT<FromT, ToT, true>;

template <typename FromT, typename ToT>
std::ostream& operator<<(std::ostream& os, const Association<FromT, ToT>& assoc) {
  return os << " id: " << assoc.id() << '\n'
            << " weight: " << assoc.getWeight() << '\n'
            << " from: " << assoc.getFrom().id() << '\n'
            << " to: " << assoc.getTo().id() << '\n';
}

} // namespace podio

#endif // PODIO_ASSOCIATION_H
