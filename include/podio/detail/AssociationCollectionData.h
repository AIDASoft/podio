#ifndef PODIO_ASSOCIATIONCOLLECTIONDATA_H
#define PODIO_ASSOCIATIONCOLLECTIONDATA_H

#include "podio/detail/AssociationFwd.h"
#include "podio/detail/AssociationObj.h"

#include "podio/CollectionBase.h"
#include "podio/CollectionBuffers.h"
#include "podio/ICollectionProvider.h"

#include <deque>
#include <memory>
#include <vector>

namespace podio {

template <typename FromT, typename ToT>
class AssociationCollectionData {
public:
  AssociationObjPointerContainer<FromT, ToT> entries{};

  AssociationCollectionData() :
      m_rel_from(new std::vector<FromT>()), m_rel_to(new std::vector<ToT>()), m_data(new AssociationDataContainer()) {
    m_refCollections.emplace_back(std::make_unique<std::vector<podio::ObjectID>>());
    m_refCollections.emplace_back(std::make_unique<std::vector<podio::ObjectID>>());
  }
  AssociationCollectionData(const AssociationCollectionData&) = delete;
  AssociationCollectionData& operator=(const AssociationCollectionData&) = delete;
  AssociationCollectionData(AssociationCollectionData&&) = default;
  AssociationCollectionData& operator=(AssociationCollectionData&&) = default;
  ~AssociationCollectionData() = default;

  podio::CollectionBuffers getCollectionBuffers(bool isSubsetColl) {
    return {isSubsetColl ? nullptr : (void*)&m_data, &m_refCollections, nullptr};
  }

  void clear(bool isSubsetColl) {
    if (isSubsetColl) {
      // We don't own the objects so no cleanup to do here
      entries.clear();
      // Clear the ObjectID I/O buffer
      for (auto& pointer : m_refCollections) {
        pointer->clear();
      }
      return;
    }

    // Normal collections manage a bit more and have to clean up a bit more
    if (m_data) {
      m_data->clear();
    }
    for (auto& pointer : m_refCollections) {
      pointer->clear();
    }
    if (m_rel_from) {
      for (auto& item : (*m_rel_from)) {
        item.unlink();
      }
      m_rel_from->clear();
    }

    if (m_rel_to) {
      for (auto& item : (*m_rel_to)) {
        item.unlink();
      }
      m_rel_to->clear();
    }

    for (auto& obj : entries) {
      delete obj;
    }
    entries.clear();
  }

  void prepareForWrite(bool isSubsetColl) {
    for (auto& pointer : m_refCollections) {
      pointer->clear();
    }

    // If this is a subset collection use the relation storing mechanism to
    // store the ObjectIDs of all reference objects and nothing else
    if (isSubsetColl) {
      for (const auto* obj : entries) {
        m_refCollections[0]->emplace_back(obj->id);
      }
      return;
    }

    m_data->reserve(entries.size());
    for (const auto obj : entries) {
      m_data->push_back(obj->weight);

      if (obj->m_from) {
        m_refCollections[0]->emplace_back(obj->m_from->getObjectID());
      } else {
        m_refCollections[0]->push_back({podio::ObjectID::invalid, podio::ObjectID::invalid});
      }

      if (obj->m_to) {
        m_refCollections[1]->emplace_back(obj->m_to->getObjectID());
      } else {
        m_refCollections[1]->push_back({podio::ObjectID::invalid, podio::ObjectID::invalid});
      }
    }
  }

  void prepareAfterRead(int collectionID) {
    int index = 0;
    for (const auto data : *m_data) {
      auto obj = new AssociationObj<FromT, ToT>({index++, collectionID}, data);
      entries.emplace_back(obj);
    }

    // Keep the I/O data buffer to keep the preparedForWrite state intact
  }

  bool setReferences(const podio::ICollectionProvider* collectionProvider, bool isSubsetColl) {
    if (isSubsetColl) {
      for (const auto& id : *m_refCollections[0]) {
        podio::CollectionBase* coll{nullptr};
        AssociationObj<FromT, ToT>* obj{nullptr};
        if (collectionProvider->get(id.collectionID, coll)) {
          auto* tmp_coll = static_cast<AssociationCollection<FromT, ToT>*>(coll);
          obj = tmp_coll->m_storage.entries[id.index];
        }
        entries.push_back(obj);
      }
      return true; // TODO: check success, how?
    }

    // Normal collections have to resolve all relations
    for (size_t i = 0; i < entries.size(); ++i) {
      const auto id = (*m_refCollections[0])[i];
      if (id.index != podio::ObjectID::invalid) {
        podio::CollectionBase* coll = nullptr;
        if (!collectionProvider->get(id.collectionID, coll)) {
          entries[i]->m_from = nullptr;
          continue;
        }
        auto* tmp_coll = static_cast<detail::GetCollT<FromT>*>(coll);
        entries[i]->m_from = new FromT((*tmp_coll)[id.index]);
      } else {
        entries[i]->m_from = nullptr;
      }
    }

    for (size_t i = 0; i < entries.size(); ++i) {
      const auto id = (*m_refCollections[1])[i];
      if (id.index != podio::ObjectID::invalid) {
        podio::CollectionBase* coll = nullptr;
        if (!collectionProvider->get(id.collectionID, coll)) {
          entries[i]->m_to = nullptr;
          continue;
        }
        auto* tmp_coll = static_cast<detail::GetCollT<ToT>*>(coll);
        entries[i]->m_to = new ToT((*tmp_coll)[id.index]);
      } else {
        entries[i]->m_to = nullptr;
      }
    }

    return true; // TODO: check success, how?
  }

  void makeSubsetCollection() {
    // Subset collections do not need all the data buffers that normal
    // collections need, so we can free them here
    m_data.reset(nullptr);

    m_rel_from.reset(nullptr);
    m_rel_to.reset(nullptr);

    // Subset collections need one vector of ObjectIDs for I/O purposes.
    m_refCollections.resize(1);
    m_refCollections[0] = std::make_unique<std::vector<podio::ObjectID>>();
  }

private:
  // members to handle relations
  podio::UVecPtr<FromT> m_rel_from{};
  podio::UVecPtr<ToT> m_rel_to{};

  // I/O related buffers (as far as necessary)
  podio::CollRefCollection m_refCollections{};
  std::unique_ptr<AssociationDataContainer> m_data{nullptr};
};

} // namespace podio

#endif // PODIO_ASSOCIATIONCOLLECTIONDATA_H
