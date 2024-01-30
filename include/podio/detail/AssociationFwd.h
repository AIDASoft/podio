#ifndef PODIO_DETAIL_ASSOCIATIONFWD_H
#define PODIO_DETAIL_ASSOCIATIONFWD_H

#include "podio/utilities/TypeHelpers.h"

#include <algorithm>
#include <deque>
#include <string>
#include <vector>

namespace podio {
namespace detail {

  /**
   * Variable template to for determining whether T is either FromT or ToT.
   * Mainly defined for convenience
   */
  template <typename T, typename FromT, typename ToT>
  static constexpr bool isFromOrToT = detail::isInTuple<T, std::tuple<FromT, ToT>>;

  /**
   * Variable template to for determining whether T is either FromT or ToT or
   * any of their mutable versions.
   */
  template <typename T, typename FromT, typename ToT>
  static constexpr bool isMutableFromOrToT =
      detail::isInTuple<T, std::tuple<FromT, ToT, GetMutableHandleType<FromT>, GetMutableHandleType<ToT>>>;

  /**
   * Get the collection type name for an AssociationCollection
   *
   * @tparam FromT the From type of the association
   * @tparam ToT the To type of the association
   * @returns a type string that is a valid c++ template instantiation
   */
  template <typename FromT, typename ToT>
  inline const std::string& associationCollTypeName() {
    const static std::string typeName =
        std::string("podio::AssociationCollection<") + FromT::TypeName + "," + ToT::TypeName + ">";
    return typeName;
  }

  /**
   * Get an SIO friendly type name for an AssociationCollection (necessary for
   * registration in the SIOBlockFactory)
   *
   * @tparam FromT the From type of the association
   * @tparam ToT the To type of
   * the association
   * @returns a string that uniquely identifies this combination
   * of From and To types
   */
  template <typename FromT, typename ToT>
  inline const std::string& associationSIOName() {
    static auto n = std::string("ASSOCIATION_FROM_") + FromT::TypeName + "_TO_" + ToT::TypeName;
    std::replace(n.begin(), n.end(), ':', '_');
    return n;
  }
} // namespace detail

// Forward declarations and typedefs used throughout the whole Association
// business
template <typename FromT, typename ToT>
class AssociationObj;

template <typename FromT, typename ToT>
using AssociationObjPointerContainer = std::deque<AssociationObj<FromT, ToT>*>;

using AssociationDataContainer = std::vector<float>;

template <typename FromT, typename ToT, bool Mutable>
class AssociationT;

template <typename FromT, typename ToT>
using Association = AssociationT<detail::GetDefaultHandleType<FromT>, detail::GetDefaultHandleType<ToT>, false>;

template <typename FromT, typename ToT>
using MutableAssociation = AssociationT<detail::GetDefaultHandleType<FromT>, detail::GetDefaultHandleType<ToT>, true>;

template <typename FromT, typename ToT>
class AssociationCollection;

template <typename FromT, typename ToT>
class AssociationCollectionData;

template <typename FromT, typename ToT, bool Mutable>
class AssociationCollectionIteratorT;

template <typename FromT, typename ToT>
using AssociationCollectionIterator = AssociationCollectionIteratorT<FromT, ToT, false>;

template <typename FromT, typename ToT>
using AssociationMutableCollectionIterator = AssociationCollectionIteratorT<FromT, ToT, true>;

} // namespace podio

namespace std {
/// Specialization for enabling structure bindings for Associations
template <typename F, typename T, bool M>
struct tuple_size<podio::AssociationT<F, T, M>> : std::integral_constant<size_t, 3> {};

/// Specialization for enabling structure bindings for Associations
template <size_t Index, typename F, typename T, bool M>
struct tuple_element<Index, podio::AssociationT<F, T, M>> : tuple_element<Index, std::tuple<F, T, float>> {};
} // namespace std

#endif // PODIO_DETAIL_ASSOCIATIONFWD_H
