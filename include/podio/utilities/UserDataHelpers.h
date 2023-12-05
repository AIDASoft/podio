#ifndef PODIO_UTILITIES_USERDATAHELPERS_H
#define PODIO_UTILITIES_USERDATAHELPERS_H

#include "podio/utilities/TypeHelpers.h"

#include <array>
#include <tuple>
#include <type_traits>
#include <typeinfo>

namespace podio {

template <typename T>
inline constexpr const char* userDataTypeName();

template <typename T>
inline constexpr const char* userDataCollTypeName();

namespace detail {

  template <typename T>
  using hasTypeName_t = decltype(T::typeName);

  template <typename T>
  using hasIsComponent_t = decltype(T::is_component);

  template <typename T>
  static constexpr bool isComponent =
      podio::det::is_detected_v<hasIsComponent_t, T> && podio::det::is_detected_v<hasTypeName_t, T>;

} // namespace detail

} // namespace podio

#endif // PODIO_UTILITIES_USERDATAHELPERS_H
