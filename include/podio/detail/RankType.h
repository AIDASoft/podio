#ifndef PODIO_DETAIL_RANKTYPE_H
#define PODIO_DETAIL_RANKTYPE_H
#include <cstdint>

namespace podio::detail {

class Rank {
public:
  Rank(void* rank) : m_rank(reinterpret_cast<value_type>(rank)) {
  }
  friend bool operator<(const Rank& lhs, const Rank& rhs) {
    return lhs.m_rank < rhs.m_rank;
  }

private:
#ifdef UINTPTR_MAX
  using value_type = uintptr_t;
#else
  using value_type = uintmax_t;
#endif
  value_type m_rank;
};
} // namespace podio::detail

#endif // PODIO_DETAIL_RANKTYPE_H
