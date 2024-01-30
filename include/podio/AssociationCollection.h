#ifndef PODIO_ASSOCIATIONCOLLECTION_H
#define PODIO_ASSOCIATIONCOLLECTION_H

#include "podio/detail/AssociationCollectionImpl.h"

// Preprocessor helper macros for concatenating tokens at preprocessing times
// Necessary because we use __COUNTER__ below for unique names of static
// variables for values returned by registration function calls
#define PODIO_PP_CONCAT_IMPL(x, y) x##y
#define PODIO_PP_CONCAT(x, y) PODIO_PP_CONCAT_IMPL(x, y)

#ifndef PODIO_ENABLE_SIO
  #define PODIO_ENABLE_SIO 0
#endif

#if PODIO_ENABLE_SIO && __has_include("podio/detail/AssociationSIOBlock.h")
  #include "podio/detail/AssociationSIOBlock.h"
  /**
   * Main macro for declaring associations. Takes care of the following things:
   * - Registering the necessary buffer creation functionality with the
   *   CollectionBufferFactory.
   * - Registering the necessary SIOBlock with the SIOBlock factory
   */
  #define PODIO_DECLARE_ASSOCIATION(FromT, ToT)                                                                        \
    const static auto PODIO_PP_CONCAT(REGISTERED_ASSOCIATION_, __COUNTER__) =                                          \
        podio::detail::registerAssociationCollection<FromT, ToT>(                                                      \
            podio::detail::associationCollTypeName<FromT, ToT>());                                                     \
    const static auto PODIO_PP_CONCAT(ASSOCIATION_SIO_BLOCK_, __COUNTER__) = podio::AssociationSIOBlock<FromT, ToT>{};
#else
  /**
   * Main macro for declaring associations. Takes care of the following things:
   * - Registering the necessary buffer creation functionality with the
   *   CollectionBufferFactory.
   */
  #define PODIO_DECLARE_ASSOCIATION(FromT, ToT)                                                                        \
    const static auto PODIO_PP_CONCAT(REGISTERED_ASSOCIATION_, __COUNTER__) =                                          \
        podio::detail::registerAssociationCollection<FromT, ToT>(                                                      \
            podio::detail::associationCollTypeName<FromT, ToT>());
#endif

#endif // PODIO_ASSOCIATIONCOLLECTION_H
