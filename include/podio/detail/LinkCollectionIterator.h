#ifndef PODIO_DETAIL_LINKCOLLECTIONITERATOR_H
#define PODIO_DETAIL_LINKCOLLECTIONITERATOR_H

#include "podio/detail/LinkFwd.h"
#include "podio/utilities/MaybeSharedPtr.h"

namespace podio {
template <typename FromT, typename ToT, bool Mutable>
class LinkCollectionIteratorT {
  using LinkType = LinkT<FromT, ToT, Mutable>;
  using LinkObjT = LinkObj<FromT, ToT>;

public:
  LinkCollectionIteratorT(size_t index, const LinkObjPointerContainer<FromT, ToT>* coll) :
      m_index(index), m_object(podio::utils::MaybeSharedPtr<LinkObjT>{nullptr}), m_collection(coll) {
  }

  LinkCollectionIteratorT(const LinkCollectionIteratorT&) = delete;
  LinkCollectionIteratorT& operator=(const LinkCollectionIteratorT&) = delete;

  bool operator!=(const LinkCollectionIteratorT& other) const {
    return m_index != other.m_index; // TODO: may not be complete
  }

  LinkType operator*() {
    m_object.m_obj = podio::utils::MaybeSharedPtr<LinkObjT>((*m_collection)[m_index]);
    return m_object;
  }

  LinkType* operator->() {
    m_object.m_obj = podio::utils::MaybeSharedPtr<LinkObjT>((*m_collection)[m_index]);
    return &m_object;
  }

  LinkCollectionIteratorT& operator++() {
    ++m_index;
    return *this;
  }

private:
  size_t m_index;
  LinkType m_object;
  const LinkObjPointerContainer<FromT, ToT>* m_collection;
};
} // namespace podio

#endif // PODIO_DETAIL_LINKCOLLECTIONITERATOR_H
