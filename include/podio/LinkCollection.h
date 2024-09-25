#ifndef PODIO_LINKCOLLECTION_H
#define PODIO_LINKCOLLECTION_H

#include "podio/detail/LinkCollectionImpl.h"
#include "podio/detail/PreprocessorMacros.h"

#ifndef PODIO_ENABLE_SIO
  #define PODIO_ENABLE_SIO 0
#endif

/// Macro for registering links at the CollectionBufferFactory by injecting the
/// corresponding buffer creation function.
#define PODIO_REGISTER_LINK_BUFFERFACTORY(FromT, ToT)                                                                  \
  const static auto PODIO_PP_CONCAT(REGISTERED_LINK_, __COUNTER__) =                                                   \
      podio::detail::registerLinkCollection<FromT, ToT>(podio::detail::linkCollTypeName<FromT, ToT>());

/// Macro for registering the necessary SIOBlock for a Link with the SIOBlock factory
#define PODIO_REGISTER_LINK_SIOFACTORY(FromT, ToT)                                                                     \
  const static auto PODIO_PP_CONCAT(LINK_SIO_BLOCK_, __COUNTER__) = podio::LinkSIOBlock<FromT, ToT>{};

#if PODIO_ENABLE_SIO && __has_include("podio/detail/LinkSIOBlock.h")
  #include "podio/detail/LinkSIOBlock.h"
  /// Main macro for declaring links. Takes care of the following things:
  /// - Registering the necessary buffer creation functionality with the
  ///   CollectionBufferFactory.
  /// - Registering the necessary SIOBlock with the SIOBlock factory
  #define PODIO_DECLARE_LINK(FromT, ToT)                                                                               \
    PODIO_REGISTER_LINK_BUFFERFACTORY(FromT, ToT)                                                                      \
    PODIO_REGISTER_LINK_SIOFACTORY(FromT, ToT)
#else
  /// Main macro for declaring links. Takes care of the following things:
  /// - Registering the necessary buffer creation functionality with the
  ///   CollectionBufferFactory.
  #define PODIO_DECLARE_LINK(FromT, ToT) PODIO_REGISTER_LINK_BUFFERFACTORY(FromT, ToT)
#endif

#endif // PODIO_LINKCOLLECTION_H
