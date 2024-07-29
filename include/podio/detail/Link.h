#ifndef PODIO_DETAIL_LINK_H
#define PODIO_DETAIL_LINK_H

#include "podio/detail/LinkFwd.h"
#include "podio/detail/LinkObj.h"
#include "podio/utilities/MaybeSharedPtr.h"
#include "podio/utilities/TypeHelpers.h"

#ifdef PODIO_JSON_OUTPUT
  #include "nlohmann/json.hpp"
#endif

#include <ostream>
#include <utility> // std::swap

namespace podio {

/// Generalized Link type for both Mutable and immutable (default)
/// versions. User facing clases with the expected naming scheme are defined via
/// template aliases are defined just below
template <typename FromT, typename ToT, bool Mutable>
class LinkT {
  // The typedefs in LinkFwd.h should make sure that at this point
  // Mutable classes are stripped, i.e. the user should never be able to trigger
  // these!
  static_assert(std::is_same_v<detail::GetDefaultHandleType<FromT>, FromT>,
                "Links need to be instantiated with the default types!");
  static_assert(std::is_same_v<detail::GetDefaultHandleType<ToT>, ToT>,
                "Links need to be instantiated with the default types!");

  using LinkObjT = LinkObj<FromT, ToT>;
  friend LinkCollection<FromT, ToT>;
  friend LinkCollectionIteratorT<FromT, ToT, Mutable>;
  friend LinkT<FromT, ToT, !Mutable>;

  /// Helper member variable to check whether FromU and ToU can be used for this
  /// Link. We need this to make SFINAE trigger in some cases below
  template <typename FromU, typename ToU>
  constexpr static bool sameTypes = std::is_same_v<FromU, FromT> && std::is_same_v<ToU, ToT>;

  /// Variable template to for determining whether T is either FromT or ToT.
  /// Mainly defined for convenience
  template <typename T>
  static constexpr bool isFromOrToT = detail::isInTuple<T, std::tuple<FromT, ToT>>;

  /// Variable template to for determining whether T is either FromT or ToT or
  /// any of their mutable versions.
  template <typename T>
  static constexpr bool isMutableFromOrToT =
      detail::isInTuple<T,
                        std::tuple<FromT, ToT, detail::GetMutableHandleType<FromT>, detail::GetMutableHandleType<ToT>>>;

public:
  using mutable_type = podio::MutableLink<FromT, ToT>;
  using value_type = podio::Link<FromT, ToT>;
  using collection_type = podio::LinkCollection<FromT, ToT>;

  /// Constructor
  LinkT() : m_obj(new LinkObjT{}, podio::utils::MarkOwned) {
  }

  /// Constructor with weight
  LinkT(float weight) : m_obj(new LinkObjT{}, podio::utils::MarkOwned) {
    m_obj->weight = weight;
  }

  /// Copy constructor
  LinkT(const LinkT& other) = default;

  /// Assignment operator
  LinkT& operator=(LinkT other) {
    swap(*this, other);
    return *this;
  }

  /// Implicit conversion of mutable to immutable links
  template <typename FromU, typename ToU, typename = std::enable_if_t<Mutable && sameTypes<FromU, ToU>>>
  operator LinkT<FromU, ToU, false>() const {
    return LinkT<FromU, ToU, false>(m_obj);
  }

  /// Create a mutable deep-copy with identical relations
  template <typename FromU = FromT, typename ToU = ToT, typename = std::enable_if_t<sameTypes<FromU, ToU>>>
  MutableLink<FromU, ToU> clone(bool cloneRelations = true) const {
    auto tmp = new LinkObjT(podio::ObjectID{}, m_obj->weight);
    if (cloneRelations) {
      if (m_obj->m_from) {
        tmp->m_from = new FromT(*m_obj->m_from);
      }
      if (m_obj->m_to) {
        tmp->m_to = new ToT(*m_obj->m_to);
      }
    }
    return MutableLink<FromU, ToU>(podio::utils::MaybeSharedPtr(tmp, podio::utils::MarkOwned));
  }

  template <bool Mut = Mutable, typename = std::enable_if_t<!Mut && !Mutable>>
  static Link<FromT, ToT> makeEmpty() {
    return {nullptr};
  }

  /// Destructor
  ~LinkT() = default;

  /// Get the weight of the link
  float getWeight() const {
    return m_obj->weight;
  }

  /// Set the weight of the link
  template <bool Mut = Mutable, typename = std::enable_if_t<Mut && Mutable>>
  void setWeight(float value) {
    m_obj->weight = value;
  }

  /// Access the related-from object
  const FromT getFrom() const {
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
  const ToT getTo() const {
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

  /// Templated version for getting an element of the link by type. Only
  /// available for Links where FromT and ToT are **not the same type**,
  /// and if the requested type is actually part of the Link. It is only
  /// possible to get the immutable types from this. Will result in a
  /// compilation error if any of these conditions is not met.
  ///
  /// @tparam T the desired type
  /// @returns T the element of the Link
  template <typename T, typename = std::enable_if_t<!std::is_same_v<ToT, FromT> && isFromOrToT<T>>>
  T get() const {
    if constexpr (std::is_same_v<T, FromT>) {
      return getFrom();
    } else {
      return getTo();
    }
  }

  /// Tuple like index based access to the elements of the Link. Returns
  /// only immutable types of the links. This method enables structured
  /// bindings for Links.
  ///
  /// @tparam Index an index (smaller than 3) to access an element of the Link
  /// @returns Depending on the value of Index:
  ///   - 0: The From element of the Link
  ///   - 1: The To element of the Link
  ///   - 2: The weight of the Link
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

  /// Templated version for setting an element of the link by type. Only
  /// available for Links where FromT and ToT are **not the same type**,
  /// and if the requested type is actually part of the Link. Will result
  /// in a compilation error if any of these conditions is not met.
  ///
  /// @tparam T type of value (**infered!**)
  /// @param value the element to set for this link.
  template <typename T, typename = std::enable_if_t<Mutable && !std::is_same_v<ToT, FromT> && isMutableFromOrToT<T>>>
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

  /// disconnect from Link instance
  void unlink() {
    m_obj = podio::utils::MaybeSharedPtr<LinkObjT>(nullptr);
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

  bool operator==(const LinkT& other) const {
    return m_obj == other.m_obj;
  }

  bool operator!=(const LinkT& other) const {
    return !(*this == other);
  }

  template <typename FromU, typename ToU, typename = std::enable_if_t<sameTypes<FromU, ToU>>>
  bool operator==(const LinkT<FromU, ToU, !Mutable>& other) const {
    return m_obj == other.m_obj;
  }

  template <typename FromU, typename ToU, typename = std::enable_if_t<sameTypes<FromU, ToU>>>
  bool operator!=(const LinkT<FromU, ToU, !Mutable>& other) const {
    return !(*this == other);
  }

  bool operator<(const LinkT& other) const {
    return m_obj < other.m_obj;
  }

  friend void swap(LinkT& a, LinkT& b) {
    using std::swap;
    swap(a.m_obj, b.m_obj); // swap out the internal pointers
  }

private:
  /// Constructor from existing LinkObj
  explicit LinkT(podio::utils::MaybeSharedPtr<LinkObjT> obj) : m_obj(std::move(obj)) {
  }

  template <bool Mut = Mutable, typename = std::enable_if_t<!Mut && !Mutable>>
  LinkT(LinkObjT* obj) : m_obj(podio::utils::MaybeSharedPtr<LinkObjT>(obj)) {
  }

  podio::utils::MaybeSharedPtr<LinkObjT> m_obj{nullptr};
}; // namespace podio

template <typename FromT, typename ToT>
std::ostream& operator<<(std::ostream& os, const Link<FromT, ToT>& link) {
  if (!link.isAvailable()) {
    return os << "[not available]";
  }

  return os << " id: " << link.id() << '\n'
            << " weight: " << link.getWeight() << '\n'
            << " from: " << link.getFrom().id() << '\n'
            << " to: " << link.getTo().id() << '\n';
}

#ifdef PODIO_JSON_OUTPUT
template <typename FromT, typename ToT>
void to_json(nlohmann::json& j, const Link<FromT, ToT>& link) {
  j = nlohmann::json{{"weight", link.getWeight()}};

  j["from"] = nlohmann::json{{"collectionID", link.getFrom().getObjectID().collectionID},
                             {"index", link.getFrom().getObjectID().index}};

  j["to"] = nlohmann::json{{"collectionID", link.getTo().getObjectID().collectionID},
                           {"index", link.getTo().getObjectID().index}};
}
#endif

} // namespace podio

#endif // PODIO_DETAIL_LINK_H
