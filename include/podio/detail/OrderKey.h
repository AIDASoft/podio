#ifndef PODIO_DETAIL_ORDERKEY_H
#define PODIO_DETAIL_ORDERKEY_H
#include <cstdint>

namespace podio::detail {

class OrderKey {
public:
  OrderKey(void* orderKey) : m_orderKey(reinterpret_cast<value_type>(orderKey)) {
  }
  friend bool operator<(const OrderKey& lhs, const OrderKey& rhs) {
    return lhs.m_orderKey < rhs.m_orderKey;
  }

private:
#ifdef UINTPTR_MAX
  using value_type = uintptr_t;
#else
  using value_type = uintmax_t;
#endif
  value_type m_orderKey;
};
} // namespace podio::detail

#endif // PODIO_DETAIL_ORDERKEY_H
