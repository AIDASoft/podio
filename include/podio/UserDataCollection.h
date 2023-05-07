#ifndef PODIO_USERDATACOLLECTION_H
#define PODIO_USERDATACOLLECTION_H

#include "podio/CollectionBase.h"
#include "podio/CollectionBuffers.h"
#include "podio/DatamodelRegistry.h"
#include "podio/utilities/TypeHelpers.h"

#include <map>
#include <string>
#include <typeindex>
#include <utility>
#include <vector>

#define PODIO_ADD_USER_TYPE(type)                                                                                      \
  template <>                                                                                                          \
  constexpr const char* userDataTypeName<type>() {                                                                     \
    return #type;                                                                                                      \
  }

namespace podio {

/** tuple of basic types supported in user vector
 */
using SupportedUserDataTypes =
    std::tuple<float, double, int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t>;

/**
 * Alias template to be used to enable template specializations only for the types listed in the
 * SupportedUserDataTypes list
 */
template <typename T>
using EnableIfSupportedUserType = std::enable_if_t<detail::isInTuple<T, SupportedUserDataTypes>>;

/** helper template to provide readable type names for basic types with macro PODIO_ADD_USER_TYPE(type)
 */
template <typename BasicType, typename = EnableIfSupportedUserType<BasicType>>
constexpr const char* userDataTypeName();

PODIO_ADD_USER_TYPE(float)
PODIO_ADD_USER_TYPE(double)

PODIO_ADD_USER_TYPE(int8_t)
PODIO_ADD_USER_TYPE(int16_t)
PODIO_ADD_USER_TYPE(int32_t)
PODIO_ADD_USER_TYPE(int64_t)
PODIO_ADD_USER_TYPE(uint8_t)
PODIO_ADD_USER_TYPE(uint16_t)
PODIO_ADD_USER_TYPE(uint32_t)
PODIO_ADD_USER_TYPE(uint64_t)

/** Collection of basic types for additional user data not defined in the EDM.
 *  The data is stored in an std::vector<basic_type>. Supported are all basic types supported in
 *  PODIO, i.e. float, double and 8-64 bit fixed size signed and unsigned integers - @see SupportedUserDataTypes.
 *  @author F.Gaede, DESY
 *  @date Sep 2021
 */
template <typename BasicType, typename = EnableIfSupportedUserType<BasicType>>
class UserDataCollection : public CollectionBase {

private:
  std::vector<BasicType> _vec{};
  // Pointer to the actual storage, necessary for I/O. In order to have
  // simpler move-semantics this will be set and properly initialized on
  // demand during the call to getBuffers
  std::vector<BasicType>* _vecPtr{nullptr};
  int m_collectionID{0};
  CollRefCollection m_refCollections{};
  VectorMembersInfo m_vecmem_info{};

public:
  UserDataCollection() = default;
  /// Constructor from an existing vector (wich will be moved from!)
  UserDataCollection(std::vector<BasicType>&& vec) : _vec(std::move(vec)) {
  }
  UserDataCollection(const UserDataCollection&) = delete;
  UserDataCollection& operator=(const UserDataCollection&) = delete;
  UserDataCollection(UserDataCollection&&) = default;
  UserDataCollection& operator=(UserDataCollection&&) = default;
  ~UserDataCollection() = default;

  /// prepare buffers for serialization
  void prepareForWrite() const override {
  }

  /// re-create collection from buffers after read
  void prepareAfterRead() override {
  }

  /// initialize references after read
  bool setReferences(const ICollectionProvider*) override {
    return true;
  }

  /// set collection ID
  void setID(unsigned id) override {
    m_collectionID = id;
  }

  /// get collection ID
  unsigned getID() const override {
    return m_collectionID;
  }

  /// Get the collection buffers for this collection
  podio::CollectionWriteBuffers getBuffers() override {
    _vecPtr = &_vec; // Set the pointer to the correct internal vector
    return {&_vecPtr, &m_refCollections, &m_vecmem_info};
  }

  podio::CollectionReadBuffers createBuffers() /*const*/ final {
    return {nullptr, nullptr, nullptr,
            [](podio::CollectionReadBuffers buffers, bool) {
              return std::make_unique<UserDataCollection<BasicType>>(std::move(*buffers.dataAsVector<BasicType>()));
            },
            [](podio::CollectionReadBuffers& buffers) {
              buffers.data = podio::CollectionWriteBuffers::asVector<BasicType>(buffers.data);
            }};
  }

  podio::CollectionReadBuffers createSchemaEvolvableBuffers(__attribute__((unused)) int readSchemaVersion,
                                                            __attribute__((unused))
                                                            podio::Backend backend) /*const*/ final {
    return createBuffers();
  }

  /// check for validity of the container after read
  bool isValid() const override {
    return true;
  }

  /// number of elements in the collection
  size_t size() const override {
    return _vec.size();
  }

  /// fully qualified type name
  std::string getTypeName() const override {
    return std::string("podio::UserDataCollection<") + userDataTypeName<BasicType>() + ">";
  }

  /// fully qualified type name of elements - with namespace
  std::string getValueTypeName() const override {
    return userDataTypeName<BasicType>();
  }

  /// fully qualified type name of stored POD elements - with namespace
  std::string getDataTypeName() const override {
    return userDataTypeName<BasicType>();
  }

  /// clear the collection and all internal states
  void clear() override {
    _vec.clear();
  };

  /// check if this collection is a subset collection - no subset possible
  bool isSubsetCollection() const override {
    return false;
  }

  /// declare this collection to be a subset collectionv - no effect
  void setSubsetCollection(bool) override {
  }

  /// The schema version is fixed manually
  SchemaVersionT getSchemaVersion() const final {
    return 1;
  }

  /// Print this collection to the passed stream
  void print(std::ostream& os = std::cout, bool flush = true) const override {
    os << "[";
    if (!_vec.empty()) {
      os << _vec[0];
      for (size_t i = 1; i < _vec.size(); ++i) {
        os << ", " << _vec[i];
      }
    }
    os << "]";

    if (flush) {
      os.flush(); // Necessary for python
    }
  }

  size_t getDatamodelRegistryIndex() const override {
    return DatamodelRegistry::NoDefinitionNecessary;
  }

  // ----- some wrapers for std::vector and access to the complete std::vector (if really needed)

  typename std::vector<BasicType>::iterator begin() {
    return _vec.begin();
  }
  typename std::vector<BasicType>::iterator end() {
    return _vec.end();
  }
  typename std::vector<BasicType>::const_iterator begin() const {
    return _vec.begin();
  }
  typename std::vector<BasicType>::const_iterator end() const {
    return _vec.end();
  }

  typename std::vector<BasicType>::reference operator[](size_t idx) {
    return _vec[idx];
  }
  typename std::vector<BasicType>::const_reference operator[](size_t idx) const {
    return _vec[idx];
  }

  void resize(size_t count) {
    _vec.resize(count);
  }
  void push_back(const BasicType& value) {
    _vec.push_back(value);
  }

  /// access to the actual data vector
  typename std::vector<BasicType>& vec() {
    return _vec;
  }

  /// const access to the actual data vector
  const typename std::vector<BasicType>& vec() const {
    return _vec;
  }
};

// don't make this macro public as it should only be used internally here...
#undef PODIO_ADD_USER_TYPE

template <typename BasicType, typename = EnableIfSupportedUserType<BasicType>>
std::ostream& operator<<(std::ostream& o, const podio::UserDataCollection<BasicType>& coll) {
  coll.print(o);
  return o;
}

} // namespace podio

#endif
