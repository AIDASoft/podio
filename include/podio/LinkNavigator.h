#ifndef PODIO_LINKNAVIGATOR_H
#define PODIO_LINKNAVIGATOR_H

#include <concepts>
#include <map>
#include <tuple>
#include <utility>
#include <vector>

namespace podio {

namespace detail::links {
  /// A small struct that simply bundles an object and its weight for a more
  /// convenient return value for the LinkNavigator
  ///
  /// @note In most uses the names of the members should not really matter as it
  /// is possible to us this via structured bindings
  template <typename T>
  struct WeightedObject {
    WeightedObject(T obj, float w) : o(obj), weight(w) {
    }
    T o;          ///< The object
    float weight; ///< The weight in the link

    bool operator==(const WeightedObject<T>& other) const {
      return other.o == o && other.weight == weight;
    }
  };

  /// Simple struct tag for overload selection in LinkNavigator below
  struct [[deprecated("The tagged versions of getLinked are deprecated use getLinkedFrom insted")]] ReturnFromTag {};
  /// Simple struct tag for overload selection in LinkNavigator below
  struct [[deprecated("The tagged versions of getLinked are deprecated use getLinkedTo insted")]] ReturnToTag {};
} // namespace detail::links

/// NOTE: This can go at the same time as the deprecated tags below are removed
#if !defined(__CLING__)
  #define INLCONSTEXPR inline constexpr
#else
  #define INLCONSTEXPR
#endif
/// Tag variable to select the lookup of *From* objects have links with a *To*
/// object in podio::LinkNavigator::getLinked
// INLCONSTEXPR detail::links::ReturnFromTag ReturnFrom;
[[deprecated("The tagged versions of getLinked are deprecated use getLinkedFrom insted")]] INLCONSTEXPR
    detail::links::ReturnFromTag ReturnFrom;
/// Tag variable to select the lookup of *To* objects that have links with a
/// *From* object in podio::LinkNavigator::getLinked
// INLCONSTEXPR detail::links::ReturnToTag ReturnTo;
[[deprecated("The tagged versions of getLinked are deprecated use getLinkedTo insted")]] INLCONSTEXPR
    detail::links::ReturnToTag ReturnTo;
#undef INLCONSTEXPR

/// A helper class to more easily handle one-to-many links.
///
/// Internally simply populates two maps in its constructor and then queries
/// them to retrieve objects that are linked with another.
///
/// @note There are no guarantees on the order of the objects in these maps.
/// Hence, there are also no guarantees on the order of the returned objects,
/// even if there inherintly is an order to them in the underlying links
/// collection.
template <typename LinkCollT>
class LinkNavigator {
  using FromT = typename LinkCollT::from_type;
  using ToT = typename LinkCollT::to_type;

  template <typename T>
  using WeightedObject = detail::links::WeightedObject<T>;

public:
  /// Construct a navigator from an link collection
  LinkNavigator(const LinkCollT& links);

  /// We do only construct from a collection
  LinkNavigator() = delete;
  LinkNavigator(const LinkNavigator&) = default;
  LinkNavigator& operator=(const LinkNavigator&) = default;
  LinkNavigator(LinkNavigator&&) = default;
  LinkNavigator& operator=(LinkNavigator&&) = default;
  ~LinkNavigator() = default;

  /// Get all the *From* objects and weights that have links with the passed
  /// object
  ///
  /// You will get this overload if you pass the podio::ReturnFrom tag as second
  /// argument
  ///
  /// @note This overload works always, even if the LinkCollection that was used
  /// to construct this instance of the LinkNavigator has the same From and To
  /// types.
  ///
  /// @param object The object that is labeled *To* in the link
  /// @param . tag variable for selecting this overload
  ///
  /// @returns A vector of all objects and their weights that have links with
  ///          the passed object
  std::vector<WeightedObject<FromT>> getLinkedFrom(const ToT& object) const {
    const auto& [begin, end] = m_to2from.equal_range(object);
    std::vector<WeightedObject<FromT>> result;
    result.reserve(std::distance(begin, end));

    for (auto it = begin; it != end; ++it) {
      result.emplace_back(it->second);
    }
    return result;
  }

  [[deprecated("Use getLinkedFrom instead")]]
  std::vector<WeightedObject<FromT>> getLinked(const ToT& object, podio::detail::links::ReturnFromTag) const {
    return getLinkedFrom(object);
  }

  [[deprecated("Use getLinkedFrom instead")]]
  std::vector<WeightedObject<FromT>> getLinked(const typename ToT::mutable_type& object,
                                               podio::detail::links::ReturnFromTag) const {
    return getLinkedFrom(ToT(object));
  }

  /// Get all the *From* objects and weights that have links with the passed
  /// object
  ///
  /// @note This overload will automatically do the right thing (TM) in case the
  /// LinkCollection that has been passed to construct this LinkNavigator has
  /// different From and To types.
  ///
  /// @param object The object that is labeled *To* in the link
  ///
  /// @returns A vector of all objects and their weights that have links with
  ///          the passed object
  template <typename ToU = ToT>
    requires(!std::same_as<FromT, ToU>)
  std::vector<WeightedObject<FromT>> getLinked(const ToT& object) const {
    return getLinkedFrom(object);
  }

  /// Overload for cppyy that makes things work with mutable handles
  template <typename ToU = ToT>
    requires(!std::same_as<FromT, ToU>)
  std::vector<WeightedObject<FromT>> getLinked(const typename ToT::mutable_type& object) const {
    return getLinkedFrom(ToT(object));
  }

  /// Get all the *To* objects and weights that have links with the passed
  /// object
  ///
  /// You will get this overload if you pass the podio::ReturnTo tag as second
  /// argument
  ///
  /// @note This overload works always, even if the LinkCollection that was used
  /// to construct this instance of the LinkNavigator has the same From and To
  /// types.
  ///
  /// @param object The object that is labeled *From* in the link
  /// @param . tag variable for selecting this overload
  ///
  /// @returns A vector of all objects and their weights that have links with
  ///          the passed object
  std::vector<WeightedObject<ToT>> getLinkedTo(const FromT& object) const {
    const auto& [begin, end] = m_from2to.equal_range(object);
    std::vector<WeightedObject<ToT>> result;
    result.reserve(std::distance(begin, end));

    for (auto it = begin; it != end; ++it) {
      result.emplace_back(it->second);
    }
    return result;
  }

  [[deprecated("Use getLinkedTo instead")]]
  std::vector<WeightedObject<ToT>> getLinked(const FromT& object, podio::detail::links::ReturnToTag) const {
    return getLinkedTo(object);
  }

  [[deprecated("Use getLinkedTo instead")]]
  std::vector<WeightedObject<ToT>> getLinked(const typename FromT::mutable_type& object,
                                             podio::detail::links::ReturnToTag) const {
    return getLinkedTo(FromT(object));
  }

  /// Get all the *To* objects and weights that have links with the passed
  /// object
  ///
  /// @note This overload will automatically do the right thing (TM) in case the
  /// LinkCollection that has been passed to construct this LinkNavigator has
  /// different From and To types.
  ///
  /// @param object The object that is labeled *From* in the link
  ///
  /// @returns A vector of all objects and their weights that have links with
  ///          the passed object
  template <typename FromU = FromT>
    requires(!std::same_as<FromU, ToT>)
  std::vector<WeightedObject<ToT>> getLinked(const FromT& object) const {
    return getLinkedTo(object);
  }

  /// Overload for cppyy that makes things work with mutable handles
  template <typename FromU = FromT>
    requires(!std::same_as<FromU, ToT>)
  std::vector<WeightedObject<ToT>> getLinked(const typename FromT::mutable_type& object) const {
    return getLinkedTo(FromT(object));
  }

private:
  std::multimap<FromT, WeightedObject<ToT>> m_from2to{}; ///< Map the from to the to objects
  std::multimap<ToT, WeightedObject<FromT>> m_to2from{}; ///< Map the to to the from objects
};

template <typename LinkCollT>
LinkNavigator<LinkCollT>::LinkNavigator(const LinkCollT& links) {
  for (const auto& [from, to, weight] : links) {
    m_from2to.emplace(std::piecewise_construct, std::forward_as_tuple(from), std::forward_as_tuple(to, weight));
    m_to2from.emplace(std::piecewise_construct, std::forward_as_tuple(to), std::forward_as_tuple(from, weight));
  }
}

} // namespace podio

#endif // PODIO_LINKNAVIGATOR_H
