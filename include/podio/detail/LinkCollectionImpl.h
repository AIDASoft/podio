#ifndef PODIO_DETAIL_LINKCOLLECTIONIMPL_H
#define PODIO_DETAIL_LINKCOLLECTIONIMPL_H

#include "podio/ObjectID.h"
#include "podio/detail/Link.h"
#include "podio/detail/LinkCollectionData.h"
#include "podio/detail/LinkCollectionIterator.h"
#include "podio/detail/LinkFwd.h"
#include "podio/detail/LinkObj.h"

#include "podio/CollectionBase.h"
#include "podio/CollectionBufferFactory.h"
#include "podio/CollectionBuffers.h"
#include "podio/DatamodelRegistry.h"
#include "podio/ICollectionProvider.h"
#include "podio/SchemaEvolution.h"
#include "podio/detail/Pythonizations.h"
#include "podio/utilities/MaybeSharedPtr.h"
#include "podio/utilities/StaticConcatenate.h"
#include "podio/utilities/TypeHelpers.h"

#include <algorithm>
#include <iterator>
#include <ranges>
#include <string_view>

#ifdef PODIO_JSON_OUTPUT
  #include "nlohmann/json.hpp"
#endif

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <iomanip>
#include <memory>
#include <mutex>
#include <ostream>
#include <string>
#include <type_traits>

namespace podio {

template <typename FromT, typename ToT>
class LinkCollection : public podio::CollectionBase {
  static_assert(std::is_same_v<FromT, detail::GetDefaultHandleType<FromT>>,
                "Links need to be instantiated with the default types!");
  static_assert(std::is_same_v<ToT, detail::GetDefaultHandleType<ToT>>,
                "Links need to be instantiated with the default types!");

  // convenience typedefs
  using CollectionDataT = podio::LinkCollectionData<FromT, ToT>;

public:
  using from_type = FromT;
  using to_type = ToT;
  using value_type = Link<FromT, ToT>;
  using mutable_type = MutableLink<FromT, ToT>;
  using const_iterator = LinkCollectionIterator<FromT, ToT>;
  using iterator = LinkMutableCollectionIterator<FromT, ToT>;
  using difference_type = ptrdiff_t;
  using size_type = size_t;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using reverse_iterator = std::reverse_iterator<iterator>;

  LinkCollection() = default;

  LinkCollection(CollectionDataT&& data, bool isSubsetColl) :
      m_isSubsetColl(isSubsetColl), m_collectionID(0), m_storage(std::move(data)) {
  }

  // Move-only type
  LinkCollection(const LinkCollection&) = delete;
  LinkCollection& operator=(const LinkCollection&) = delete;
  LinkCollection(LinkCollection&&) = default;
  LinkCollection& operator=(LinkCollection&&) = default;
#if defined(__cpp_lib_containers_ranges)
  /// Constructor from range in order to enable the use of std::ranges::to see
  /// @ref from.
  template <detail::RangeConvertibleTo<value_type> R>
  LinkCollection(std::from_range_t, R&& range) : LinkCollection() {
    if constexpr (detail::RangeOf<R, value_type>) {
      setSubsetCollection();
    }

    for (auto&& elem : range) {
      push_back(std::forward<decltype(elem)>(elem));
    }
  }
#endif

  ~LinkCollection() override {
    // Need to tell the storage how to clean up
    m_storage.clear(m_isSubsetColl);
  }

  /// Construct a LinkCollection from a range of mutable or immutable links
  ///
  /// If the range is over mutable Link handles, the collection will (try) to
  /// take ownership of the handles. If the range is over immutable Link handles
  /// the collection will be a subset collection and not take ownership of the
  /// handles.
  ///
  /// @tparam R A range type (at least std::input_range) that contains for which
  ///           the range_value_t is convertible to an immutable link.
  /// @param  range The range from which the collection should be constructed
  ///
  /// @returns  A LinkCollection populated with the values from the range
  ///
  /// @throws std::invalid_argument if a range of immutable handles that are not
  ///           yet owned by a collection
  template <detail::RangeConvertibleTo<value_type> R>
  static LinkCollection from(R&& range) {
    LinkCollection coll;
    if constexpr (detail::RangeOf<R, value_type>) {
      coll.setSubsetCollection();
    }

    // NOTE: We cannot use ranges::copy here, because that will always resolve
    // to only one of the push_back overloads, specifically the one taking
    // value_type. See, e.g.: https://godbolt.org/z/seqdh79rr
    // Additionally, we want no reference in the loop variable to make sure we
    // get the correct handle type depending on the range we pass in
    for (auto&& elem : range) {
      coll.push_back(std::forward<decltype(elem)>(elem));
    }

    return coll;
  }

  /// Append a new link to the collection and return this object
  mutable_type create() {
    if (m_isSubsetColl) {
      throw std::logic_error("Cannot create new elements on a subset collection");
    }

    auto obj = m_storage.entries.emplace_back(new LinkObj<FromT, ToT>());
    obj->id = {int(m_storage.entries.size() - 1), m_collectionID};
    return mutable_type(podio::utils::MaybeSharedPtr(obj));
  }

  /// Returns the immutable object of given index
  value_type operator[](std::size_t index) const {
    return value_type(m_storage.entries[index]);
  }
  /// Returns the mutable object of given index
  mutable_type operator[](std::size_t index) {
    return mutable_type(podio::utils::MaybeSharedPtr(m_storage.entries[index]));
  }
  /// Returns the immutable object of given index
  value_type at(std::size_t index) const {
    return value_type(m_storage.entries.at(index));
  }
  /// Returns the mutable object of given index
  mutable_type at(std::size_t index) {
    return mutable_type(podio::utils::MaybeSharedPtr(m_storage.entries.at(index)));
  }

  void push_back(mutable_type object) {
    // We have to do different things here depending on whether this is a
    // subset collection or not. A normal collection cannot collect objects
    // that are already part of another collection, while a subset collection
    // can only collect such objects
    if (!m_isSubsetColl) {
      auto obj = object.m_obj;
      if (obj->id.index == podio::ObjectID::untracked) {
        obj->id = {static_cast<int>(m_storage.entries.size()), m_collectionID};
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
    const auto maxStorage = m_storage.entries.max_size();
    if (!m_isSubsetColl) {
      // non-subset collections shouldn't have more elements than the maximum index of ObjectID
      const auto maxIndex = std::numeric_limits<decltype(ObjectID::index)>::max();
      return std::min<size_t>(maxIndex, maxStorage);
    }
    return maxStorage;
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
  // reverse iterators
  reverse_iterator rbegin() {
    return reverse_iterator(end());
  }
  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(end());
  }
  const_reverse_iterator crbegin() const {
    return rbegin();
  }
  reverse_iterator rend() {
    return reverse_iterator(begin());
  }
  const_reverse_iterator rend() const {
    return const_reverse_iterator(begin());
  }
  const_reverse_iterator crend() const {
    return rend();
  }

  /// check if the collection has a valid ID
  bool hasID() const override {
    return getID() != static_cast<uint32_t>(podio::ObjectID::untracked) &&
        getID() != static_cast<uint32_t>(podio::ObjectID::invalid);
  }

  [[deprecated("isValid will be removed, use hasID() if you want to check if it has an ID, otherwise assume the "
               "collection is valid")]]
  bool isValid() const override {
    return hasID();
  }

  podio::CollectionWriteBuffers getBuffers() override {
    return m_storage.getCollectionBuffers(m_isSubsetColl);
  }

  constexpr static std::string_view typeName =
      podio::utils::static_concatenate_v<podio::detail::link_coll_name_prefix, FromT::typeName,
                                         podio::detail::link_name_infix, ToT::typeName,
                                         podio::detail::link_name_suffix>;

  constexpr static std::string_view valueTypeName = value_type::typeName;
  constexpr static std::string_view dataTypeName = "podio::LinkData";

  const std::string_view getTypeName() const override {
    return typeName;
  }

  const std::string_view getValueTypeName() const override {
    return valueTypeName;
  }

  const std::string_view getDataTypeName() const override {
    return dataTypeName;
  }

  /// Cppyy protocol to setup the pythonizations for this class. Not to be called directly.
  static void __cppyy_pythonize__(PyObject* klass, const std::string& name) {
    podio::detail::pythonizations::pythonize_subscript(klass, name);
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

  void setID(uint32_t id) override {
    m_collectionID = id;
    if (!m_isSubsetColl) {
      std::ranges::for_each(m_storage.entries, [id](auto* obj) { obj->id = {obj->id.index, id}; });
    }
  }

  uint32_t getID() const override {
    return m_collectionID;
  }

  void prepareForWrite() const override {
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

  mutable bool m_isPrepared{false};
  bool m_isSubsetColl{false};
  uint32_t m_collectionID{0};
  mutable std::unique_ptr<std::mutex> m_storageMtx{std::make_unique<std::mutex>()};
  mutable CollectionDataT m_storage{};
};

namespace detail {
  template <typename FromT, typename ToT>
  podio::CollectionReadBuffers createLinkBuffers(bool subsetColl) {
    auto readBuffers = podio::CollectionReadBuffers{};
    readBuffers.type = podio::LinkCollection<FromT, ToT>::typeName;
    readBuffers.schemaVersion = podio::LinkCollection<FromT, ToT>::schemaVersion;
    readBuffers.data = subsetColl ? nullptr : new LinkDataContainer();

    // Either it is a subset collection or we have two relations
    const auto nRefs = subsetColl ? 1 : 2;
    readBuffers.references = new podio::CollRefCollection(nRefs);
    for (auto& ref : *readBuffers.references) {
      // Make sure to place usable buffer pointers here
      ref = std::make_unique<std::vector<podio::ObjectID>>();
    }

    readBuffers.createCollection = [](const podio::CollectionReadBuffers& buffers, bool isSubsetColl) {
      LinkCollectionData<FromT, ToT> data(buffers, isSubsetColl);
      return std::make_unique<LinkCollection<FromT, ToT>>(std::move(data), isSubsetColl);
    };

    readBuffers.deleteBuffers = [](const podio::CollectionReadBuffers& buffers) {
      if (buffers.data) {
        // If we have data then we are not a subset collection and we have
        // to clean up all type erased buffers by casting them back to
        // something that we can delete
        delete static_cast<LinkDataContainer*>(buffers.data);
      }
      delete buffers.references;
      delete buffers.vectorMembers;
    };

    return readBuffers;
  }

  template <typename FromT, typename ToT>
  bool registerLinkCollection(const std::string_view linkTypeName) {
    const static auto reg = [&linkTypeName]() {
      const auto schemaVersion = LinkCollection<FromT, ToT>::schemaVersion;

      auto& factory = CollectionBufferFactory::mutInstance();
      factory.registerCreationFunc(std::string(linkTypeName), schemaVersion, createLinkBuffers<FromT, ToT>);

      // For now passing the same schema version for from and current version
      // simply to make SchemaEvolution aware of LinkCollections
      podio::SchemaEvolution::mutInstance().registerEvolutionFunc(std::string(linkTypeName), schemaVersion,
                                                                  schemaVersion, SchemaEvolution::noOpSchemaEvolution,
                                                                  SchemaEvolution::Priority::AutoGenerated);

      return true;
    }();
    return reg;
  }
} // namespace detail

#if defined(PODIO_JSON_OUTPUT) && !defined(__CLING__)
template <typename FromT, typename ToT>
void to_json(nlohmann::json& j, const podio::LinkCollection<FromT, ToT>& collection) {
  j = nlohmann::json::array();
  for (auto&& elem : collection) {
    j.emplace_back(elem);
  }
}
#endif

} // namespace podio

template <typename FromT, typename ToT>
struct fmt::formatter<podio::LinkCollection<FromT, ToT>> {
  constexpr auto parse(fmt::format_parse_context& ctx) {
    auto it = ctx.begin();
    if (it != ctx.end() && *it != '}') {
      fmt::throw_format_error("Invalid format. LinkCollections do not support specifiers");
    }
    return it;
  }

  auto format(const podio::LinkCollection<FromT, ToT>& coll, fmt::format_context& ctx) const {
    auto out = ctx.out();

    out = fmt::format_to(out, "          id:      weight:\n");
    for (const auto&& elem : coll) {
      out = fmt::format_to(out, "{}  {:+12e}\n", elem.id(), elem.getWeight());
      out = fmt::format_to(out, "     from : {}\n       to : {}\n", elem.getFrom().id(), elem.getTo().id());
    }

    return out;
  }
};

// Disable fmt's range formatter for LinkCollection to avoid ambiguity with the
// custom formatter above
template <typename FromT, typename ToT>
struct fmt::is_range<podio::LinkCollection<FromT, ToT>, char> : std::false_type {};

namespace podio {
template <typename FromT, typename ToT>
std::ostream& operator<<(std::ostream& o, const LinkCollection<FromT, ToT>& v) {
  fmt::format_to(std::ostreambuf_iterator<char>(o), "{}", v);
  return o;
}
} // namespace podio

#endif // PODIO_DETAIL_LINKCOLLECTIONIMPL_H
