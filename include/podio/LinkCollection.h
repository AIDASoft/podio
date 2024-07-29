#ifndef PODIO_LINKCOLLECTION_H
#define PODIO_LINKCOLLECTION_H

#include "podio/detail/LinkCollectionImpl.h"

// Preprocessor helper macros for concatenating tokens at preprocessing times
// Necessary because we use __COUNTER__ below for unique names of static
// variables for values returned by registration function calls
#define PODIO_PP_CONCAT_IMPL(x, y) x##y
#define PODIO_PP_CONCAT(x, y) PODIO_PP_CONCAT_IMPL(x, y)

#ifndef PODIO_ENABLE_SIO
  #define PODIO_ENABLE_SIO 0
#endif

#if PODIO_ENABLE_SIO && __has_include("podio/detail/LinkSIOBlock.h")
  #include "podio/detail/LinkSIOBlock.h"
  /// Main macro for declaring links. Takes care of the following things:
  /// - Registering the necessary buffer creation functionality with the
  ///   CollectionBufferFactory.
  /// - Registering the necessary SIOBlock with the SIOBlock factory
  #define PODIO_DECLARE_LINK(FromT, ToT)                                                                               \
    const static auto PODIO_PP_CONCAT(REGISTERED_LINK_, __COUNTER__) =                                                 \
        podio::detail::registerLinkCollection<FromT, ToT>(podio::detail::linkCollTypeName<FromT, ToT>());              \
    const static auto PODIO_PP_CONCAT(LINK_SIO_BLOCK_, __COUNTER__) = podio::LinkSIOBlock<FromT, ToT>{};
#else
  /// Main macro for declaring links. Takes care of the following things:
  /// - Registering the necessary buffer creation functionality with the
  ///   CollectionBufferFactory.
  #define PODIO_DECLARE_LINK(FromT, ToT)                                                                               \
    const static auto PODIO_PP_CONCAT(REGISTERED_LINK_, __COUNTER__) =                                                 \
        podio::detail::registerLinkCollection<FromT, ToT>(podio::detail::linkCollTypeName<FromT, ToT>());
#endif

#endif // PODIO_LINKCOLLECTION_H
