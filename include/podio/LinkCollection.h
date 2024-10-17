#ifndef PODIO_LINKCOLLECTION_H
#define PODIO_LINKCOLLECTION_H

#include "podio/detail/LinkCollectionImpl.h"
#include "podio/detail/PreprocessorMacros.h"

#ifndef PODIO_ENABLE_SIO
  #define PODIO_ENABLE_SIO 0
#endif

/// Main macro for declaring links. Takes care of registering the necessary
/// buffer creation functionality with the CollectionBufferFactory.
#define PODIO_DECLARE_LINK(FromT, ToT)                                                                                 \
  const static auto PODIO_PP_CONCAT(REGISTERED_LINK_, __COUNTER__) =                                                   \
      podio::detail::registerLinkCollection<FromT, ToT>(podio::detail::linkCollTypeName<FromT, ToT>());

#if PODIO_ENABLE_SIO && __has_include("podio/detail/LinkSIOBlock.h")
  #include <podio/detail/LinkSIOBlock.h>
  /// Macro for registering the necessary SIOBlock for a Link with the SIOBlock factory
  #define PODIO_DECLARE_LINK_SIO(FromT, ToT)                                                                           \
    const static auto PODIO_PP_CONCAT(LINK_SIO_BLOCK_, __COUNTER__) = podio::LinkSIOBlock<FromT, ToT>{};
#endif

#endif // PODIO_LINKCOLLECTION_H
