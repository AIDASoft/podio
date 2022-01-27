#ifndef PODIO_ASSOCIATION_H
#define PODIO_ASSOCIATION_H

#include "podio/AssociationFwd.h"
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
  // The typedefs in AssociationFwd.h should make sure that at this point
  // Mutable classes are stripped, i.e. the user should never be able to trigger
  // these!
  static_assert(std::is_same_v<detail::GetDefT<FromT>, FromT>,
                "Associations need to be instantiated with the default types!");
  static_assert(std::is_same_v<detail::GetDefT<ToT>, ToT>,
                "Associations need to be instantiated with the default types!");

  using AssociationObjT = AssociationObj<FromT, ToT>;

public:
  /// Constructor
  AssociationT() : m_obj(new AssociationObjT()) {
    m_obj->acquire();
  }

  /// Constructor with weight
  AssociationT(float weight) : m_obj(new AssociationObjT()) {
    m_obj->acquire();
    m_obj->weight = weight;
  }

  /// Constructor from existing AssociationObj
  AssociationT(AssociationObjT* obj) : m_obj(obj) {
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

  /// Implicit conversion of mutable to immutable associations
  template <typename FromU, typename ToU, bool Mut = Mutable,
            typename = std::enable_if_t<Mut && std::is_same_v<FromU, FromT> && std::is_same_v<ToU, ToT>>>
  operator AssociationT<FromU, ToU, false>() const {
    return AssociationT<FromU, ToU, false>(m_obj);
  }

  /// Create a mutable deep-copy with identical relations
  template <typename FromU = FromT, typename ToU = ToT,
            typename = std::enable_if_t<std::is_same_v<FromU, FromT> && std::is_same_v<ToU, ToT>>>
  MutableAssociation<FromU, ToU> clone() const {
    return {new AssociationObjT(*m_obj)};
  }

  /// Destructor
  ~AssociationT() {
    if (m_obj) {
      m_obj->release();
    }
  }

  /// Get the weight of the association
  float getWeight() const {
    return m_obj->weight;
  }

  /// Set the weight of the association
  template <bool Mut = Mutable, typename = std::enable_if_t<Mut>>
  void setWeight(float value) {
    m_obj->weight = value;
  }

  /// Access the related-from object
  FromT getFrom() const {
    if (!m_obj->m_from) {
      return FromT(nullptr);
    }
    return FromT(*(m_obj->m_from));
  }

  /// Set the related-from object
  template <typename FromU, bool Mut = Mutable,
            typename = std::enable_if_t<Mut && std::is_same_v<detail::GetDefT<FromU>, FromT>>>
  void setFrom(FromU value) {
    delete m_obj->m_from;
    m_obj->m_from = new detail::GetDefT<FromU>(value);
  }

  /// Access the related-to object
  ToT getTo() const {
    if (!m_obj->m_to) {
      return ToT(nullptr);
    }
    return ToT(*(m_obj->m_to));
  }

  /// Set the related-to object
  template <typename ToU, bool Mut = Mutable,
            typename = std::enable_if_t<Mut && std::is_same_v<detail::GetDefT<ToU>, ToT>>>
  void setTo(ToU value) {
    delete m_obj->m_to;
    m_obj->m_to = new detail::GetDefT<ToU>(value);
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
std::ostream& operator<<(std::ostream& os, const Association<FromT, ToT>& assoc) {
  return os << " id: " << assoc.id() << '\n'
            << " weight: " << assoc.getWeight() << '\n'
            << " from: " << assoc.getFrom().id() << '\n'
            << " to: " << assoc.getTo().id() << '\n';
}

} // namespace podio

#endif // PODIO_ASSOCIATION_H
