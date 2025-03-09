#ifndef PODIO_DETAIL_LINKFWD_H
#define PODIO_DETAIL_LINKFWD_H

#include "podio/utilities/TypeHelpers.h"

#include <algorithm>
#include <deque>
#include <string>
#include <string_view>
#include <vector>

namespace podio {
namespace detail {

  /// Get the collection type name for a LinkCollection
  ///
  /// @tparam FromT the From type of the link
  /// @tparam ToT the To type of the link
  /// @returns a type string that is a valid c++ template instantiation
  template <typename FromT, typename ToT>
  inline const std::string_view linkCollTypeName() {
    const static std::string typeName =
        std::string("podio::LinkCollection<") + std::string(FromT::typeName) + "," + std::string(ToT::typeName) + ">";
    return std::string_view{typeName};
  }

  /// Get the value type name for a LinkCollection
  ///
  /// @tparam FromT the From type of the link
  /// @tparam ToT the To type of the link
  /// @returns a type string that is a valid c++ template instantiation
  template <typename FromT, typename ToT>
  inline const std::string_view linkTypeName() {
    const static std::string typeName =
        std::string("podio::Link<") + std::string(FromT::typeName) + "," + std::string(ToT::typeName) + ">";
    return std::string_view{typeName};
  }

  /// Get an SIO friendly type name for a LinkCollection (necessary for
  /// registration in the SIOBlockFactory)
  ///
  /// @tparam FromT the From type of the link
  /// @tparam ToT the To type of the link
  /// @returns a string that uniquely identifies this combination of From and To
  /// types
  template <typename FromT, typename ToT>
  inline const std::string& linkSIOName() {
    static auto n = std::string("LINK_FROM_") + std::string(FromT::typeName) + "_TO_" + std::string(ToT::typeName);
    std::replace(n.begin(), n.end(), ':', '_');
    return n;
  }
} // namespace detail

// Forward declarations and typedefs used throughout the whole Link business
template <typename FromT, typename ToT>
class LinkObj;

template <typename FromT, typename ToT>
using LinkObjPointerContainer = std::deque<LinkObj<FromT, ToT>*>;

/// Simple struct to keep implementation more in line with generated links and
/// to ease evolution of generated links into templated ones
struct LinkData {
  float weight{};
};

using LinkDataContainer = std::vector<LinkData>;

template <typename FromT, typename ToT, bool Mutable>
class LinkT;

template <typename FromT, typename ToT>
using Link = LinkT<detail::GetDefaultHandleType<FromT>, detail::GetDefaultHandleType<ToT>, false>;

template <typename FromT, typename ToT>
using MutableLink = LinkT<detail::GetDefaultHandleType<FromT>, detail::GetDefaultHandleType<ToT>, true>;

template <typename FromT, typename ToT>
class LinkCollection;

template <typename FromT, typename ToT>
class LinkCollectionData;

template <typename FromT, typename ToT, bool Mutable>
class LinkCollectionIteratorT;

template <typename FromT, typename ToT>
using LinkCollectionIterator = LinkCollectionIteratorT<FromT, ToT, false>;

template <typename FromT, typename ToT>
using LinkMutableCollectionIterator = LinkCollectionIteratorT<FromT, ToT, true>;

} // namespace podio

namespace std {
/// Specialization for enabling structured bindings for Links
template <typename F, typename T, bool M>
struct tuple_size<podio::LinkT<F, T, M>> : std::integral_constant<size_t, 3> {};

/// Specialization for enabling structured bindings for Links
template <size_t Index, typename F, typename T, bool M>
struct tuple_element<Index, podio::LinkT<F, T, M>> : tuple_element<Index, std::tuple<F, T, float>> {};
} // namespace std

#endif // PODIO_DETAIL_LINKFWD_H
