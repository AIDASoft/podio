#ifndef PODIO_RELATIONRANGE_H
#define PODIO_RELATIONRANGE_H

#include <ranges>
#include <vector>

namespace podio {
/// A simple helper class that allows one to return related objects in a way that
/// makes it possible to use the return type in a range-based for loop.
template <typename ReferenceType>
class RelationRange {
public:
  using ConstIteratorType = typename std::vector<ReferenceType>::const_iterator;

  RelationRange() = delete;

  RelationRange(ConstIteratorType begin, ConstIteratorType end) :
      m_begin(begin), m_end(end), m_size(std::distance(m_begin, m_end)) {
  }

  /// begin of the range (necessary for range-based for loop)
  ConstIteratorType begin() const {
    return m_begin;
  }
  /// end of the range (necessary for range-based for loop)
  ConstIteratorType end() const {
    return m_end;
  }
  /// convenience overload for size
  size_t size() const {
    return m_size;
  }
  /// convenience overload to check if the range is empty
  bool empty() const {
    return m_begin == m_end;
  }
  /// Indexed access
  ReferenceType operator[](size_t i) const {
    auto it = m_begin;
    std::advance(it, i);
    return *it;
  }
  /// Indexed access with range check
  ReferenceType at(size_t i) const {
    if (i < m_size) {
      auto it = m_begin;
      std::advance(it, i);
      return *it;
    }
    throw std::out_of_range("index out of bounds for RelationRange");
  }

private:
  ConstIteratorType m_begin;
  ConstIteratorType m_end;
  size_t m_size{0};
};
} // namespace podio

// Opt-in to view concept
template <typename ReferenceType>
inline constexpr bool std::ranges::enable_view<podio::RelationRange<ReferenceType>> = true;
// Opt-in to borrowed_range concept
template <typename ReferenceType>
inline constexpr bool std::ranges::enable_borrowed_range<podio::RelationRange<ReferenceType>> = true;

#endif // PODIO_RELATIONRANGE_H
