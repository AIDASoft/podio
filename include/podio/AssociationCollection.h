#ifndef PODIO_ASSOCIATIONCOLLECTION_H
#define PODIO_ASSOCIATIONCOLLECTION_H

#include "podio/detail/AssociationCollectionData.h"
#include "podio/detail/AssociationFwd.h"
#include "podio/detail/AssociationObj.h"

#include "podio/Association.h"
#include "podio/AssociationCollectionIterator.h"
#include "podio/CollectionBase.h"
#include "podio/CollectionBuffers.h"
#include "podio/DatamodelRegistry.h"
#include "podio/ICollectionProvider.h"
#include "podio/SchemaEvolution.h"

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
  static_assert(std::is_same_v<FromT, detail::GetDefT<FromT>>,
                "Associations need to be instantiated with the default types!");
  static_assert(std::is_same_v<ToT, detail::GetDefT<ToT>>,
                "Associations need to be instantiated with the default types!");

  // convenience typedefs
  using AssocT = Association<FromT, ToT>;
  using MutableAssocT = MutableAssociation<FromT, ToT>;

  using CollectionT = podio::AssociationCollection<FromT, ToT>;
  using CollectionDataT = podio::AssociationCollectionData<FromT, ToT>;

public:
  using const_iterator = AssociationCollectionIterator<FromT, ToT>;
  using iterator = AssociationMutableCollectionIterator<FromT, ToT>;

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
    // Need the storage how to clean up
    m_storage.clear(m_isSubsetColl);
  }

  /// Append a new association to the collection and return this object
  MutableAssocT create() {
    if (m_isSubsetColl) {
      throw std::logic_error("Cannot create new elements on a subset collection");
    }

    auto obj = m_storage.entries.emplace_back(new AssociationObj<FromT, ToT>());
    obj->id = {int(m_storage.entries.size() - 1), m_collectionID};
    return MutableAssocT(obj);
  }

  /// Returns the immutable object of given index
  AssocT operator[](unsigned int index) const {
    return AssocT(m_storage.entries[index]);
  }
  /// Returns the mutable object of given index
  MutableAssocT operator[](unsigned int index) {
    return MutableAssocT(m_storage.entries[index]);
  }
  /// Returns the immutable object of given index
  AssocT at(unsigned int index) const {
    return AssocT(m_storage.entries.at(index));
  }
  /// Returns the mutable object of given index
  MutableAssocT at(unsigned int index) {
    return MutableAssocT(m_storage.entries.at(index));
  }

  void push_back(AssocT object) {
    // We have to do different things here depending on whether this is a
    // subset collection or not. A normal collection cannot collect objects
    // that are already part of another collection, while a subset collection
    // can only collect such objects
    if (!m_isSubsetColl) {
      auto obj = object.m_obj;
      if (obj->id.index == podio::ObjectID::untracked) {
        const auto size = m_storage.entries.size();
        obj->id = {(int)size, m_collectionID};
        m_storage.entries.push_back(obj);
      } else {
        throw std::invalid_argument("Object already in a collection. Cannot add it to a second collection");
      }
    } else {
      const auto obj = object.m_obj;
      if (obj->id.index < 0) {
        throw std::invalid_argument(
            "Objects can only be stored in a subset collection if they are already elements of a collection");
      }
      m_storage.entries.push_back(obj);
      // No need to handle any relations here, since this is already done by the
      // "owning" collection
    }
  }

  /// Number of elements in the collection
  size_t size() const override {
    return m_storage.entries.size();
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
  const_iterator end() const {
    return const_iterator(m_storage.entries.size(), &m_storage.entries);
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

  // podio::CollectionReadBuffers createBuffers() override /*const*/ {
  //   // Very cumbersome way at the moment. We get the actual buffers to have the
  //   // references and vector members sized appropriately (we will use this
  //   // information to create new buffers outside)
  //   auto collBuffers = m_storage.getCollectionBuffers(m_isSubsetColl);
  //   auto readBuffers = podio::CollectionReadBuffers{};
  //   readBuffers.references = collBuffers.references;
  //   readBuffers.vectorMembers = collBuffers.vectorMembers;
  //   readBuffers.createCollection = [](podio::CollectionReadBuffers buffers, bool isSubsetColl) {
  //     CollectionDataT data(buffers, isSubsetColl);
  //     return std::make_unique<CollectionT>(std::move(data), isSubsetColl);
  //   };
  //   readBuffers.recast = [](podio::CollectionReadBuffers& buffers) {
  //     if (buffers.data) {
  //       buffers.data = podio::CollectionWriteBuffers::asVector<float>(buffers.data);
  //     }
  //   };
  //   return readBuffers;
  // }

  std::string getTypeName() const override {
    return std::string("podio::AssociationCollection<") + FromT::TypeName + "," + ToT::TypeName + ">";
  }

  std::string getValueTypeName() const override {
    return podio::detail::associationSIOName<FromT, ToT>();
  }

  std::string getDataTypeName() const override {
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
  int m_collectionID{0};
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

} // namespace podio

#endif // PODIO_ASSOCIATIONCOLLECTION_H
