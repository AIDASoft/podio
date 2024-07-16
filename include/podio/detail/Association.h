#ifndef PODIO_DETAIL_ASSOCIATION_H
#define PODIO_DETAIL_ASSOCIATION_H

#include "podio/detail/AssociationFwd.h"
#include "podio/detail/AssociationObj.h"
#include "podio/utilities/MaybeSharedPtr.h"
#include "podio/utilities/TypeHelpers.h"

#ifdef PODIO_JSON_OUTPUT
  #include "nlohmann/json.hpp"
#endif

#include <ostream>
#include <utility> // std::swap

namespace podio {

/// Generalized Association type for both Mutable and immutable (default)
/// versions. User facing clases with the expected naming scheme are defined via
/// template aliases are defined just below
template <typename FromT, typename ToT, bool Mutable>
class AssociationT {
  // The typedefs in AssociationFwd.h should make sure that at this point
  // Mutable classes are stripped, i.e. the user should never be able to trigger
  // these!
  static_assert(std::is_same_v<detail::GetDefaultHandleType<FromT>, FromT>,
                "Associations need to be instantiated with the default types!");
  static_assert(std::is_same_v<detail::GetDefaultHandleType<ToT>, ToT>,
                "Associations need to be instantiated with the default types!");

  using AssociationObjT = AssociationObj<FromT, ToT>;
  friend AssociationCollection<FromT, ToT>;
  friend AssociationCollectionIteratorT<FromT, ToT, Mutable>;
  friend AssociationT<FromT, ToT, !Mutable>;

public:
  using mutable_type = podio::MutableAssociation<FromT, ToT>;
  using value_type = podio::Association<FromT, ToT>;
  using collection_type = podio::AssociationCollection<FromT, ToT>;

  /// Constructor
  AssociationT() : m_obj(new AssociationObjT(), podio::utils::MarkOwned) {
  }

  /// Constructor with weight
  AssociationT(float weight) : m_obj(new AssociationObjT(), podio::utils::MarkOwned) {
    m_obj->weight = weight;
  }

  /// Copy constructor
  AssociationT(const AssociationT& other) = default;

  /// Assignment operator
  AssociationT& operator=(AssociationT other) {
    swap(*this, other);
    return *this;
  }

  /// Implicit conversion of mutable to immutable associations
  template <typename FromU, typename ToU,
            typename = std::enable_if_t<Mutable && std::is_same_v<FromU, FromT> && std::is_same_v<ToU, ToT>>>
  operator AssociationT<FromU, ToU, false>() const {
    return AssociationT<FromU, ToU, false>(m_obj);
  }

  /// Create a mutable deep-copy with identical relations
  template <typename FromU = FromT, typename ToU = ToT,
            typename = std::enable_if_t<std::is_same_v<FromU, FromT> && std::is_same_v<ToU, ToT>>>
  MutableAssociation<FromU, ToU> clone() const {
    return MutableAssociation<FromU, ToU>(
        podio::utils::MaybeSharedPtr(new AssociationObjT(*m_obj), podio::utils::MarkOwned));
  }

  static Association<FromT, ToT> makeEmpty() {
    return {nullptr};
  }

  /// Destructor
  ~AssociationT() = default;

  /// Get the weight of the association
  float getWeight() const {
    return m_obj->weight;
  }

  /// Set the weight of the association
  template <bool Mut = Mutable, typename = std::enable_if_t<Mut && Mutable>>
  void setWeight(float value) {
    m_obj->weight = value;
  }

  /// Access the related-from object
  FromT getFrom() const {
    if (!m_obj->m_from) {
      return FromT::makeEmpty();
    }
    return FromT(*(m_obj->m_from));
  }

  /// Set the related-from object
  template <typename FromU,
            typename = std::enable_if_t<Mutable && std::is_same_v<detail::GetDefaultHandleType<FromU>, FromT>>>
  void setFrom(FromU value) {
    delete m_obj->m_from;
    m_obj->m_from = new detail::GetDefaultHandleType<FromU>(value);
  }

  /// Access the related-to object
  ToT getTo() const {
    if (!m_obj->m_to) {
      return ToT::makeEmpty();
    }
    return ToT(*(m_obj->m_to));
  }

  /// Set the related-to object
  template <typename ToU,
            typename = std::enable_if_t<Mutable && std::is_same_v<detail::GetDefaultHandleType<ToU>, ToT>>>
  void setTo(ToU value) {
    delete m_obj->m_to;
    m_obj->m_to = new detail::GetDefaultHandleType<ToU>(value);
  }

  /// Templated version for getting an element of the association by type. Only
  /// available for Associations where FromT and ToT are **not the same type**,
  /// and if the requested type is actually part of the Association. It is only
  /// possible to get the immutable types from this. Will result in a
  /// compilation error if any of these conditions is not met.
  ///
  /// @tparam T the desired type
  /// @returns T the element of the Association
  template <typename T, typename = std::enable_if_t<!std::is_same_v<ToT, FromT> && detail::isFromOrToT<T, FromT, ToT>>>
  T get() const {
    if constexpr (std::is_same_v<T, FromT>) {
      return getFrom();
    } else {
      return getTo();
    }
  }

  /// Tuple like index based access to the elements of the Association. Returns
  /// only immutable types of the associations. This method enables structured
  /// bindings for Associations.
  ///
  /// @tparam Index an index (smaller than 3) to access an element of the Association
  /// @returns Depending on the value of Index:
  ///   - 0: The From element of the Association
  ///   - 1: The To element of the Association
  ///   - 2: The weight of the Association
  template <size_t Index, typename = std::enable_if_t<(Index < 3)>>
  auto get() const {
    if constexpr (Index == 0) {
      return getFrom();
    } else if constexpr (Index == 1) {
      return getTo();
    } else {
      return getWeight();
    }
  }

  /// Templated version for setting an element of the association by type. Only
  /// available for Associations where FromT and ToT are **not the same type**,
  /// and if the requested type is actually part of the Association. Will result
  /// in a compilation error if any of these conditions is not met.
  ///
  /// @tparam T type of value (**infered!**)
  /// @param value the element to set for this association.
  template <
      typename T,
      typename = std::enable_if_t<Mutable && !std::is_same_v<ToT, FromT> && detail::isMutableFromOrToT<T, FromT, ToT>>>
  void set(T value) {
    if constexpr (std::is_same_v<T, FromT>) {
      setFrom(std::move(value));
    } else {
      setTo(std::move(value));
    }
  }

  /// check whether the object is actually available
  bool isAvailable() const {
    return m_obj;
  }

  /// disconnect from Association instance
  void unlink() {
    m_obj = podio::utils::MaybeSharedPtr<AssociationObjT>(nullptr);
  }

  /// Get the ObjectID
  podio::ObjectID getObjectID() const {
    if (m_obj) {
      return m_obj->id;
    }
    return podio::ObjectID{};
  }

  podio::ObjectID id() const {
    return getObjectID();
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
  /// Constructor from existing AssociationObj
  explicit AssociationT(podio::utils::MaybeSharedPtr<AssociationObjT> obj) : m_obj(std::move(obj)) {
  }
  AssociationT(AssociationObjT* obj) : m_obj(podio::utils::MaybeSharedPtr<AssociationObjT>(obj)) {
  }

  podio::utils::MaybeSharedPtr<AssociationObjT> m_obj{nullptr};
}; // namespace podio

template <typename FromT, typename ToT>
std::ostream& operator<<(std::ostream& os, const Association<FromT, ToT>& assoc) {
  if (!assoc.isAvailable()) {
    return os << "[not available]";
  }

  return os << " id: " << assoc.id() << '\n'
            << " weight: " << assoc.getWeight() << '\n'
            << " from: " << assoc.getFrom().id() << '\n'
            << " to: " << assoc.getTo().id() << '\n';
}

#ifdef PODIO_JSON_OUTPUT
template <typename FromT, typename ToT>
void to_json(nlohmann::json& j, const Association<FromT, ToT>& assoc) {
  j = nlohmann::json{{"weight", assoc.getWeight()}};

  j["from"] = nlohmann::json{{"collectionID", assoc.getFrom().getObjectID().collectionID},
                             {"index", assoc.getFrom().getObjectID().index}};

  j["to"] = nlohmann::json{{"collectionID", assoc.getTo().getObjectID().collectionID},
                           {"index", assoc.getTo().getObjectID().index}};
}
#endif

} // namespace podio

#endif // PODIO_DETAIL_ASSOCIATION_H
