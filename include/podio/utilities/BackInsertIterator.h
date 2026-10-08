#ifndef PODIO_UTILITIES_BACKINSERTITERATOR_H
#define PODIO_UTILITIES_BACKINSERTITERATOR_H

#include <cstddef>
#include <iterator>
#include <memory>
#include <utility>

namespace podio {

/// An output iterator that forwards assigned handles to a collection's push_back.
///
/// Unlike std::back_insert_iterator, this iterator preserves the handle type,
/// allowing mutable handles (e.g. clones) to be inserted into owning collections.
/// The collection's existing ownership and subset insertion rules still apply.
/// The destination collection must outlive the iterator.
template <typename Collection>
class BackInsertIterator {
public:
  using container_type = Collection;
  using iterator_category = std::output_iterator_tag;
  using value_type = void;
  using difference_type = std::ptrdiff_t;
  using pointer = void;
  using reference = void;

  BackInsertIterator() = default;

  explicit BackInsertIterator(Collection& collection) noexcept : m_collection(std::addressof(collection)) {
  }

  template <typename T>
    requires requires(Collection& collection, T&& value) { collection.push_back(std::forward<T>(value)); }
  BackInsertIterator& operator=(T&& value) {
    m_collection->push_back(std::forward<T>(value));
    return *this;
  }

  BackInsertIterator& operator*() noexcept {
    return *this;
  }

  BackInsertIterator& operator++() noexcept {
    return *this;
  }

  BackInsertIterator operator++(int) noexcept {
    return *this;
  }

private:
  Collection* m_collection{nullptr};
};

/// Create an output iterator that preserves the type of handles passed to push_back.
template <typename Collection>
BackInsertIterator<Collection> back_inserter(Collection& collection) noexcept {
  return BackInsertIterator<Collection>(collection);
}

} // namespace podio

#endif // PODIO_UTILITIES_BACKINSERTITERATOR_H
