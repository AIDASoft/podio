#ifndef PODIO_DETAIL_ASSOCIATIONFWD_H
#define PODIO_DETAIL_ASSOCIATIONFWD_H

#include <algorithm>
#include <deque>
#include <string>
#include <vector>

namespace podio {
namespace detail {
  template <typename T>
  struct GetMutType {
    using type = typename T::MutT;
  };

  template <typename T>
  using GetMutT = typename GetMutType<T>::type;

  template <typename T>
  struct GetDefType {
    using type = typename T::DefT;
  };

  template <typename T>
  using GetDefT = typename GetDefType<T>::type;

  /**
   * Helper template struct and type-alias to retrieve the collection type from
   * a given data type
   */
  template <typename T>
  struct GetCollType {
    using type = typename T::CollT;
  };

  template <typename T>
  using GetCollT = typename GetCollType<T>::type;

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
using Association = AssociationT<detail::GetDefT<FromT>, detail::GetDefT<ToT>, false>;

template <typename FromT, typename ToT>
using MutableAssociation = AssociationT<detail::GetDefT<FromT>, detail::GetDefT<ToT>, true>;

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

#endif // PODIO_DETAIL_ASSOCIATIONFWD_H
