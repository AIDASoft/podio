#ifndef PODIO_DETAIL_ASSOCIATIONCOLLECTIONIMPL_H
#define PODIO_DETAIL_ASSOCIATIONCOLLECTIONIMPL_H

#include "podio/detail/Association.h"
#include "podio/detail/AssociationCollectionData.h"
#include "podio/detail/AssociationCollectionIterator.h"
#include "podio/detail/AssociationFwd.h"
#include "podio/detail/AssociationObj.h"

#include "podio/CollectionBase.h"
#include "podio/CollectionBufferFactory.h"
#include "podio/CollectionBuffers.h"
#include "podio/DatamodelRegistry.h"
#include "podio/ICollectionProvider.h"
#include "podio/SchemaEvolution.h"
#include "podio/utilities/MaybeSharedPtr.h"
#include "podio/utilities/TypeHelpers.h"

#ifdef PODIO_JSON_OUTPUT
  #include "nlohmann/json.hpp"
#endif

#include <iomanip>
#include <memory>
#include <mutex>
#include <ostream>
#include <string>
#include <type_traits>

namespace podio {

template <typename FromT, typename ToT>
class AssociationCollection : public podio::CollectionBase {
  static_assert(std::is_same_v<FromT, detail::GetDefaultHandleType<FromT>>,
                "Associations need to be instantiated with the default types!");
  static_assert(std::is_same_v<ToT, detail::GetDefaultHandleType<ToT>>,
                "Associations need to be instantiated with the default types!");

  // convenience typedefs
  using CollectionDataT = podio::AssociationCollectionData<FromT, ToT>;

public:
  using value_type = Association<FromT, ToT>;
  using mutable_type = MutableAssociation<FromT, ToT>;
  using const_iterator = AssociationCollectionIterator<FromT, ToT>;
  using iterator = AssociationMutableCollectionIterator<FromT, ToT>;
  using difference_type = ptrdiff_t;
  using size_type = size_t;

  AssociationCollection() = default;

  AssociationCollection(CollectionDataT&& data, bool isSubsetColl) :
      m_isSubsetColl(isSubsetColl), m_collectionID(0), m_storage(std::move(data)) {
  }

  // Move-only type
  AssociationCollection(const AssociationCollection&) = delete;
  AssociationCollection& operator=(const AssociationCollection&) = delete;
  AssociationCollection(AssociationCollection&&) = default;
  AssociationCollection& operator=(AssociationCollection&&) = default;

  ~AssociationCollection() {
    // Need to tell the storage how to clean up
    m_storage.clear(m_isSubsetColl);
  }

  /// Append a new association to the collection and return this object
  mutable_type create() {
    if (m_isSubsetColl) {
      throw std::logic_error("Cannot create new elements on a subset collection");
    }

    auto obj = m_storage.entries.emplace_back(new AssociationObj<FromT, ToT>());
    obj->id = {int(m_storage.entries.size() - 1), m_collectionID};
    return mutable_type(podio::utils::MaybeSharedPtr(obj));
  }

  /// Returns the immutable object of given index
  value_type operator[](unsigned int index) const {
    return value_type(m_storage.entries[index]);
  }
  /// Returns the mutable object of given index
  mutable_type operator[](unsigned int index) {
    return mutable_type(podio::utils::MaybeSharedPtr(m_storage.entries[index]));
  }
  /// Returns the immutable object of given index
  value_type at(unsigned int index) const {
    return value_type(m_storage.entries.at(index));
  }
  /// Returns the mutable object of given index
  mutable_type at(unsigned int index) {
    return mutable_type(podio::utils::MaybeSharedPtr(m_storage.at(index)));
  }

  void push_back(mutable_type object) {
    // We have to do different things here depending on whether this is a
    // subset collection or not. A normal collection cannot collect objects
    // that are already part of another collection, while a subset collection
    // can only collect such objects
    if (!m_isSubsetColl) {
      auto obj = object.m_obj;
      if (obj->id.index == podio::ObjectID::untracked) {
        const auto size = m_storage.entries.size();
        obj->id = {(int)size, m_collectionID};
        m_storage.entries.push_back(obj.release());
      } else {
        throw std::invalid_argument("Object already in a collection. Cannot add it to a second collection");
      }

    } else {
      push_back(value_type(object));
    }
  }

  void push_back(value_type object) {
    if (!m_isSubsetColl) {
      throw std::invalid_argument("Can only add immutable objects to subset collections");
    }
    auto obj = object.m_obj;
    if (obj->id.index < 0) {
      throw std::invalid_argument(
          "Object needs to be tracked by another collection in order for it to be storable in a subset collection");
    }
    m_storage.entries.push_back(obj.release());
  }

  /// Number of elements in the collection
  size_t size() const override {
    return m_storage.entries.size();
  }

  /// maximal number of elements in the collection
  std::size_t max_size() const override {
    return m_storage.entries.max_size();
  }

  /// Is the collection empty
  bool empty() const override {
    return m_storage.entries.empty();
  }

  void clear() override {
    m_storage.clear(m_isSubsetColl);
    m_isPrepared = false;
  }

  void print(std::ostream& os = std::cout, bool flush = true) const override {
    os << *this;
    if (flush) {
      os.flush();
    }
  }

  // support for the iterator protocol
  const_iterator begin() const {
    return const_iterator(0, &m_storage.entries);
  }
  const_iterator cbegin() const {
    return begin();
  }
  const_iterator end() const {
    return const_iterator(m_storage.entries.size(), &m_storage.entries);
  }
  const_iterator cend() const {
    return end();
  }
  iterator begin() {
    return iterator(0, &m_storage.entries);
  }
  iterator end() {
    return iterator(m_storage.entries.size(), &m_storage.entries);
  }

  bool isValid() const override {
    return m_isValid;
  }

  podio::CollectionWriteBuffers getBuffers() override {
    return m_storage.getCollectionBuffers(m_isSubsetColl);
  }

  const std::string_view getTypeName() const override {
    return podio::detail::associationCollTypeName<FromT, ToT>();
  }

  const std::string_view getValueTypeName() const override {
    return podio::detail::associationTypeName<FromT, ToT>();
  }

  const std::string_view getDataTypeName() const override {
    return "float";
  }

  bool isSubsetCollection() const override {
    return m_isSubsetColl;
  }

  void setSubsetCollection(bool setSubset = true) override {
    if (m_isSubsetColl != setSubset && !m_storage.entries.empty()) {
      throw std::logic_error("Cannot change the character of a collection that already contains elements");
    }

    if (setSubset) {
      m_storage.makeSubsetCollection();
    }
    m_isSubsetColl = setSubset;
  }

  void setID(unsigned id) override {
    m_collectionID = id;
  }

  unsigned getID() const override {
    return m_collectionID;
  }

  void prepareForWrite() const override {
    // TODO: Replace this double locking pattern with an atomic and only one
    // lock. Problem: std::atomic is not default movable
    {
      std::lock_guard lock{*m_storageMtx};
      // If the collection has been prepared already there is nothing to do
      if (m_isPrepared) {
        return;
      }
    }

    std::lock_guard lock{*m_storageMtx};
    // by the time we acquire the lock another thread might have already
    // succeeded, so we need to check again now
    if (m_isPrepared) {
      return;
    }
    m_storage.prepareForWrite(m_isSubsetColl);
    m_isPrepared = true;
  }

  void prepareAfterRead() override {
    // No need to go through this again if we have already done it
    if (m_isPrepared) {
      return;
    }

    if (!m_isSubsetColl) {
      // Subset collections do not store any data that would require post-processing
      m_storage.prepareAfterRead(m_collectionID);
    }
    // Preparing a collection doesn't affect the underlying I/O buffers, so this
    // collection is still prepared
    m_isPrepared = true;
  }

  bool setReferences(const ICollectionProvider* collectionProvider) override {
    return m_storage.setReferences(collectionProvider, m_isSubsetColl);
  }

  static constexpr SchemaVersionT schemaVersion = 1;

  SchemaVersionT getSchemaVersion() const override {
    return schemaVersion;
  }

  size_t getDatamodelRegistryIndex() const override {
    return podio::DatamodelRegistry::NoDefinitionNecessary;
  }

private:
  // For setReferences, we need to give our own CollectionData access to our
  // private entries. Otherwise we would need to expose a public member function
  // that gives access to the Obj* which is definitely not what we want
  friend CollectionDataT;

  bool m_isValid{false};
  mutable bool m_isPrepared{false};
  bool m_isSubsetColl{false};
  uint32_t m_collectionID{0};
  mutable std::unique_ptr<std::mutex> m_storageMtx{std::make_unique<std::mutex>()};
  mutable CollectionDataT m_storage{};
};

template <typename FromT, typename ToT>
std::ostream& operator<<(std::ostream& o, const AssociationCollection<FromT, ToT>& v) {
  const auto old_flags = o.flags();
  o << "          id:      weight:" << '\n';
  for (const auto&& el : v) {
    o << std::scientific << std::showpos << std::setw(12) << el.id() << " " << std::setw(12) << " " << el.getWeight()
      << '\n';

    o << "     from : ";
    o << el.getFrom().id() << std::endl;
    o << "       to : ";
    o << el.getTo().id() << std::endl;
  }

  o.flags(old_flags);
  return o;
}

#ifdef PODIO_JSON_OUTPUT
template <typename FromT, typename ToT>
void to_json(nlohmann::json& j, const AssociationCollection<FromT, ToT>& collection) {
  j = nlohmann::json::array();
  for (auto&& elem : collection) {
    j.emplace_back(elem);
  }
}
#endif

namespace detail {
  template <typename FromT, typename ToT>
  podio::CollectionReadBuffers createAssociationBuffers(bool subsetColl) {
    auto readBuffers = podio::CollectionReadBuffers{};
    readBuffers.data = subsetColl ? nullptr : new AssociationDataContainer();

    // Either it is a subset collection or we have two relations
    const auto nRefs = subsetColl ? 1 : 2;
    readBuffers.references = new podio::CollRefCollection(nRefs);
    for (auto& ref : *readBuffers.references) {
      // Make sure to place usable buffer pointers here
      ref = std::make_unique<std::vector<podio::ObjectID>>();
    }

    readBuffers.createCollection = [](podio::CollectionReadBuffers buffers, bool isSubsetColl) {
      AssociationCollectionData<FromT, ToT> data(buffers, isSubsetColl);
      return std::make_unique<AssociationCollection<FromT, ToT>>(std::move(data), isSubsetColl);
    };

    readBuffers.recast = [](podio::CollectionReadBuffers& buffers) {
      if (buffers.data) {
        buffers.data = podio::CollectionWriteBuffers::asVector<float>(buffers.data);
      }
    };

    readBuffers.deleteBuffers = [](podio::CollectionReadBuffers& buffers) {
      if (buffers.data) {
        // If we have data then we are not a subset collection and we have
        // to clean up all type erased buffers by casting them back to
        // something that we can delete
        delete static_cast<AssociationDataContainer*>(buffers.data);
      }
      delete buffers.references;
      delete buffers.vectorMembers;
    };

    return readBuffers;
  }

  template <typename FromT, typename ToT>
  bool registerAssociationCollection(const std::string_view assocTypeName) {
    const static auto reg = [&assocTypeName]() {
      const auto schemaVersion = AssociationCollection<FromT, ToT>::schemaVersion;

      auto& factory = CollectionBufferFactory::mutInstance();
      factory.registerCreationFunc(std::string(assocTypeName), schemaVersion, createAssociationBuffers<FromT, ToT>);

      // For now passing the same schema version for from and current version
      // simply to make SchemaEvolution aware of AssociationCollections
      podio::SchemaEvolution::mutInstance().registerEvolutionFunc(std::string(assocTypeName), schemaVersion,
                                                                  schemaVersion, SchemaEvolution::noOpSchemaEvolution,
                                                                  SchemaEvolution::Priority::AutoGenerated);

      return true;
    }();
    return reg;
  }
} // namespace detail

} // namespace podio

#endif // PODIO_DETAIL_ASSOCIATIONCOLLECTIONIMPL_H
