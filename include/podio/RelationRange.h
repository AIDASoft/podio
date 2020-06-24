#ifndef PODIO_RELATIONRANGE_H
#define PODIO_RELATIONRANGE_H

#include <vector>
#include <iterator>

namespace podio {
  /**
   * A simple helper class that allows to return related objects in a way that
   * makes it possible to use the return type in a range-based for loop.
   */
  template<typename ReferenceType>
  class RelationRange {
  public:
    using ConstIteratorType = typename std::vector<ReferenceType>::const_iterator;
    RelationRange(ConstIteratorType begin, ConstIteratorType end) : m_begin(begin), m_end(end) {}

    /// begin of the range (necessary for range-based for loop)
    ConstIteratorType begin() const { return m_begin; }
    /// end of the range (necessary for range-based for loop)
    ConstIteratorType end() const { return m_end; }
    /// convenience overload for size
    size_t size() const { return std::distance(m_begin, m_end); }
  private:
    ConstIteratorType m_begin;
    ConstIteratorType m_end;
  };
}

#endif // PODIO_RELATIONRANGE_H
