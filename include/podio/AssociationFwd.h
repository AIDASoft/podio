#ifndef PODIO_ASSOCIATIONFWD_H
#define PODIO_ASSOCIATIONFWD_H

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

} // namespace detail

// Forward declarations for a bit less typing below
template <typename FromT, typename ToT, bool Mutable>
class AssociationT;

template <typename FromT, typename ToT>
using Association = AssociationT<detail::GetDefT<FromT>, detail::GetDefT<ToT>, false>;

template <typename FromT, typename ToT>
using MutableAssociation = AssociationT<detail::GetDefT<FromT>, detail::GetDefT<ToT>, true>;
} // namespace podio

#endif // PODIO_ASSOCIATIONFWD_H
