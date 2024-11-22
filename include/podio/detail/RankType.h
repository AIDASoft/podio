#ifndef PODIO_DETAIL_RANKTYPE_H
#define PODIO_DETAIL_RANKTYPE_H
#include <cstdint>

namespace podio::detail {
#ifdef UINTPTR_MAX
using rank_type = uintptr_t;
#else
using rank_type = uintmax_t;
#endif
} // namespace podio::detail

#endif // PODIO_DETAIL_RANKTYPE_H
