#ifndef PODIO_DETAIL_ORDERKEY_H
#define PODIO_DETAIL_ORDERKEY_H
#include <functional>
namespace podio::detail {
/// Internal class allowing datatype objects to be placed in data structures like maps and sets by providing a way to
/// compare them. The comparison is based on addresses of their internal data objects.
///
/// This class is intended to be used as the return value of internal `podio::detail::getOrderKey` free functions. These
/// functions are friends of each datatype, allowing them to access the internal data objects and define less-than
/// comparison operators for both datatypes and interface types.
///
/// The friend free function design is used in order to reduce the coupling between interfaces and datatypes. Interfaces
/// do not need to be friends of datatypes to define the less-than comparison operator, which allows using datatypes
/// from different datamodels in an interface type.
class OrderKey {
public:
  OrderKey(const void* orderKey) noexcept : m_orderKey(orderKey) {
  }
  friend bool operator<(const OrderKey& lhs, const OrderKey& rhs) noexcept {
    return std::less<const void*>{}(lhs.m_orderKey, rhs.m_orderKey);
  }

private:
  const void* m_orderKey;
};
} // namespace podio::detail

#endif // PODIO_DETAIL_ORDERKEY_H
