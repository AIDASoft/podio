#ifndef PODIO_UTILITIES_USERDATAHELPERS_H
#define PODIO_UTILITIES_USERDATAHELPERS_H

#include "podio/utilities/TypeHelpers.h"

#include <array>
#include <cstdint>
#include <tuple>
#include <type_traits>
#include <typeinfo>

namespace podio::detail {

/**
 * helper template to provide readable names for types used in
 * UserDataCollections. Implementations need to be populated for all types
 * that should be usable in UserDataCollections.
 */
template <typename T>
constexpr const char* userDataTypeName();

/**
 * helper template to provide the fullly qualified type name of the
 * UserDataCollection. Implementations need to be populated for all types that
 * should be usable in UserDataCollections.
 */
template <typename T>
constexpr const char* userDataCollTypeName();

/**
 * Detector for checking whether a type T has a typeName static constexpr member
 * variable
 */
template <typename T>
using hasTypeName_t = decltype(T::typeName);

/**
 * detector for checking whether a type T has an is_component static constexpr
 * member variable
 */
template <typename T>
using hasIsComponent_t = decltype(T::is_component);

/**
 * Static bool to determine whether a type T is a component of an EDM generated
 * by podio.
 *
 * Effectively boils down to checking that a typeName and an is_component member
 * variable are present without checking their contents any further.
 */
template <typename T>
static constexpr bool isComponent =
    podio::det::is_detected_v<hasIsComponent_t, T> && podio::det::is_detected_v<hasTypeName_t, T>;

} // namespace podio::detail

#define PODIO_ADD_USER_TYPE(type)                                                                                      \
  template <>                                                                                                          \
  constexpr const char* podio::detail::userDataTypeName<type>() {                                                      \
    return #type;                                                                                                      \
  }                                                                                                                    \
  template <>                                                                                                          \
  constexpr const char* podio::detail::userDataCollTypeName<type>() {                                                  \
    return "podio::UserDataCollection<" #type ">";                                                                     \
  }

PODIO_ADD_USER_TYPE(float)
PODIO_ADD_USER_TYPE(double)
PODIO_ADD_USER_TYPE(int8_t)
PODIO_ADD_USER_TYPE(int16_t)
PODIO_ADD_USER_TYPE(int32_t)
PODIO_ADD_USER_TYPE(int64_t)
PODIO_ADD_USER_TYPE(uint8_t)
PODIO_ADD_USER_TYPE(uint16_t)
PODIO_ADD_USER_TYPE(uint32_t)
PODIO_ADD_USER_TYPE(uint64_t)

// don't make this macro public as it should only be used internally here...
#undef PODIO_ADD_USER_TYPE

#endif // PODIO_UTILITIES_USERDATAHELPERS_H
