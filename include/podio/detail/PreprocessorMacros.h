#ifndef PODIO_DETAIL_PREPROCESSORMACROS_H
#define PODIO_DETAIL_PREPROCESSORMACROS_H

// Preprocessor helper macros for concatenating tokens at preprocessing times
// Necessary because we use __COUNTER__ below for unique names of static
// variables for values returned by registration function calls
#define PODIO_PP_CONCAT_IMPL(x, y) x##y
#define PODIO_PP_CONCAT(x, y) PODIO_PP_CONCAT_IMPL(x, y)

#endif // PODIO_DETAIL_PREPROCESSORMACROS_H
