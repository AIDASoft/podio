#ifndef PODIO_DETAIL_ORDERKEY_H
#define PODIO_DETAIL_ORDERKEY_H
#include <cstdint>

namespace podio::detail {
// Internal class allowing datatype objects to be placed in data structures like maps and sets by providing a way to
// compare them. The comparison is based on a numeric value derived from the address of their internal data objects.
//
// This class is intended to be used as the return value of internal `podio::detail::getOrderKey` free functions. These
// functions are friends of each datatype, allowing them to access the internal data objects and define less-than
// comparison operators for both datatypes and interface types.
//
// The friend free function design is used in order to reduce the coupling between interfaces and datatypes. Interfaces
// do not need to be friends of datatypes to define the less-than comparison operator, which allows using datatypes from
// different datamodels in an interface type.
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
