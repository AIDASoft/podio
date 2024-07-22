#ifndef PODIO_DETAIL_ASSOCIATIONFWD_H
#define PODIO_DETAIL_ASSOCIATIONFWD_H

#include "podio/utilities/TypeHelpers.h"

#include <algorithm>
#include <deque>
#include <string>
#include <string_view>
#include <vector>

namespace podio {
namespace detail {

  /// Get the collection type name for an AssociationCollection
  ///
  /// @tparam FromT the From type of the association
  /// @tparam ToT the To type of the association
  /// @returns a type string that is a valid c++ template instantiation
  template <typename FromT, typename ToT>
  inline const std::string_view associationCollTypeName() {
    const static std::string typeName =
        std::string("podio::AssociationCollection<") + FromT::typeName + "," + ToT::typeName + ">";
    return std::string_view{typeName};
  }

  /// Get the value type name for an AssociationCollection
  ///
  /// @tparam FromT the From type of the association
  /// @tparam ToT the To type of the association
  /// @returns a type string that is a valid c++ template instantiation
  template <typename FromT, typename ToT>
  inline const std::string_view associationTypeName() {
    const static std::string typeName =
        std::string("podio::Association<") + FromT::typeName + "," + ToT::typeName + ">";
    return std::string_view{typeName};
  }

  /// Get an SIO friendly type name for an AssociationCollection (necessary for
  /// registration in the SIOBlockFactory)
  ///
  /// @tparam FromT the From type of the association
  /// @tparam ToT the To type of
  /// the association
  /// @returns a string that uniquely identifies this combination
  /// of From and To types
  template <typename FromT, typename ToT>
  inline const std::string& associationSIOName() {
    static auto n = std::string("ASSOCIATION_FROM_") + FromT::typeName + "_TO_" + ToT::typeName;
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
