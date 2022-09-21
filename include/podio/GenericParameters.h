// -*- C++ -*-
#ifndef PODIO_GENERICPARAMETERS_H
#define PODIO_GENERICPARAMETERS_H 1

#include "podio/utilities/TypeHelpers.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#define DEPRECATED_ACCESS [[deprecated("Use templated access functionality")]]

namespace podio {

/// The types which are supported in the GenericParameters
using SupportedGenericDataTypes = std::tuple<int, float, std::string>;

/// Static bool for determining if a type T is a supported GenericParamter type
template <typename T>
static constexpr bool isSupportedGenericDataType = detail::isAnyOrVectorOf<T, SupportedGenericDataTypes>;

/// Alias template to be used for enabling / disabling template overloads that
/// should only be present for actually supported data types
template <typename T>
using EnableIfValidGenericDataType = typename std::enable_if_t<isSupportedGenericDataType<T>>;

namespace detail {
  /// Helper struct to determine how to return different types from the
  /// GenericParamters to avoid unnecessary copies but also to prohibit carrying
  /// around const references to ints or floats
  template <typename T>
  struct GenericDataReturnTypeHelper {
    using type = T;
  };

  /// Specialization for std::string. Those will always be returned by const ref
  template <>
  struct GenericDataReturnTypeHelper<std::string> {
    using type = const std::string&;
  };

  /// Specialization for std::vector. Those will always be returned by const ref
  template <typename T>
  struct GenericDataReturnTypeHelper<std::vector<T>> {
    using type = const std::vector<T>&;
  };
} // namespace detail

/// Alias template for determining the appropriate return type for the passed in
/// type
template <typename T>
using GenericDataReturnType = typename detail::GenericDataReturnTypeHelper<T>::type;

// These should be trivial to remove once the deprecated non-templated access
// functionality is actually removed
typedef std::vector<int> IntVec;
typedef std::vector<float> FloatVec;
typedef std::vector<std::string> StringVec;

/** GenericParameters objects allow to store generic named parameters of type
 *  int, float and string or vectors of these types.
 *  They can be used  to store (user) meta data that is
 *  run, event or collection dependent.
 *  (based on lcio::LCParameters)
 *
 * @author F. Gaede, DESY
 * @date Apr 2020
 */

class GenericParameters {
public:
  template <typename T>
  using MapType = std::map<std::string, std::vector<T>>;

private:
  using IntMap = MapType<int>;
  using FloatMap = MapType<float>;
  using StringMap = MapType<std::string>;
  // need mutex pointers for having the possibility to copy/move GenericParameters
  using MutexPtr = std::unique_ptr<std::mutex>;

public:
  GenericParameters() = default;

  /// GenericParameters are copyable
  /// NOTE: This is currently mainly done to keep the ROOT I/O happy, because
  /// that needs a copy constructor
  GenericParameters(const GenericParameters&);
  GenericParameters& operator=(const GenericParameters&) = delete;

  /// GenericParameters are default moveable
  GenericParameters(GenericParameters&&) = default;
  GenericParameters& operator=(GenericParameters&&) = default;

  ~GenericParameters() = default;

  /// Get the value that is stored under the given key, by const reference or by
  /// value depending on the desired type
  template <typename T, typename = EnableIfValidGenericDataType<T>>
  GenericDataReturnType<T> getValue(const std::string&) const;

  /// Store (a copy of) the passed value under the given key
  template <typename T, typename = EnableIfValidGenericDataType<T>>
  void setValue(const std::string& key, T value);

  /// Overload for catching const char* setting for string values
  void setValue(const std::string& key, std::string value) {
    setValue<std::string>(key, std::move(value));
  }

  /// Overload for catching initializer list setting for vector values
  template <typename T, typename = std::enable_if_t<detail::isInTuple<T, SupportedGenericDataTypes>>>
  void setValue(const std::string& key, std::initializer_list<T>&& values) {
    setValue<std::vector<T>>(key, std::move(values));
  }

  /// Get the number of elements stored under the given key for a type
  template <typename T, typename = EnableIfValidGenericDataType<T>>
  size_t getN(const std::string& key) const;

  /// Get all available keys for a given type
  template <typename T, typename = EnableIfValidGenericDataType<T>>
  std::vector<std::string> getKeys() const;

  /** Returns the first integer value for the given key.
   */
  DEPRECATED_ACCESS int getIntVal(const std::string& key) const;

  /** Returns the first float value for the given key.
   */
  DEPRECATED_ACCESS float getFloatVal(const std::string& key) const;

  /** Returns the first string value for the given key.
   */
  DEPRECATED_ACCESS const std::string& getStringVal(const std::string& key) const;

  /** Adds all integer values for the given key to values.
   *  Returns a reference to values for convenience.
   */
  DEPRECATED_ACCESS IntVec& getIntVals(const std::string& key, IntVec& values) const;

  /** Adds all float values for the given key to values.
   *  Returns a reference to values for convenience.
   */
  DEPRECATED_ACCESS FloatVec& getFloatVals(const std::string& key, FloatVec& values) const;

  /** Adds all float values for the given key to values.
   *  Returns a reference to values for convenience.
   */
  DEPRECATED_ACCESS StringVec& getStringVals(const std::string& key, StringVec& values) const;

  /** Returns a list of all keys of integer parameters.
   */
  DEPRECATED_ACCESS const StringVec& getIntKeys(StringVec& keys) const;

  /** Returns a list of all keys of float parameters.
   */
  DEPRECATED_ACCESS const StringVec& getFloatKeys(StringVec& keys) const;

  /** Returns a list of all keys of string parameters.
   */
  DEPRECATED_ACCESS const StringVec& getStringKeys(StringVec& keys) const;

  /** The number of integer values stored for this key.
   */
  DEPRECATED_ACCESS int getNInt(const std::string& key) const;

  /** The number of float values stored for this key.
   */
  DEPRECATED_ACCESS int getNFloat(const std::string& key) const;

  /** The number of string values stored for this key.
   */
  DEPRECATED_ACCESS int getNString(const std::string& key) const;

  /** Set integer values for the given key.
   */
  DEPRECATED_ACCESS void setValues(const std::string& key, const IntVec& values);

  /** Set float values for the given key.
   */
  DEPRECATED_ACCESS void setValues(const std::string& key, const FloatVec& values);

  /** Set string values for the given key.
   */
  DEPRECATED_ACCESS void setValues(const std::string& key, const StringVec& values);

  /// erase all elements
  void clear() {
    _intMap.clear();
    _floatMap.clear();
    _stringMap.clear();
  }

  void print(std::ostream& os = std::cout, bool flush = true);

  /// Check if no parameter is stored (i.e. if all internal maps are empty)
  bool empty() const {
    return _intMap.empty() && _floatMap.empty() && _stringMap.empty();
  }

  /**
   * Get the internal int map (necessary for serialization with SIO)
   */
  const IntMap& getIntMap() const {
    return getMap<int>();
  }
  IntMap& getIntMap() {
    return getMap<int>();
  }

  /**
   * Get the internal float map (necessary for serialization with SIO)
   */
  const FloatMap& getFloatMap() const {
    return getMap<float>();
  }
  FloatMap& getFloatMap() {
    return getMap<float>();
  }

  /**
   * Get the internal string map (necessary for serialization with SIO)
   */
  const StringMap& getStringMap() const {
    return getMap<std::string>();
  }
  StringMap& getStringMap() {
    return getMap<std::string>();
  }

private:
  /// Get a reference to the internal map for a given type (necessary for SIO)
  template <typename T>
  const MapType<detail::GetVectorType<T>>& getMap() const {
    if constexpr (std::is_same_v<detail::GetVectorType<T>, int>) {
      return _intMap;
    } else if constexpr (std::is_same_v<detail::GetVectorType<T>, float>) {
      return _floatMap;
    } else {
      return _stringMap;
    }
  }

  /// Get a reference to the internal map for a given type (necessary for SIO)
  template <typename T>
  MapType<detail::GetVectorType<T>>& getMap() {
    if constexpr (std::is_same_v<detail::GetVectorType<T>, int>) {
      return _intMap;
    } else if constexpr (std::is_same_v<detail::GetVectorType<T>, float>) {
      return _floatMap;
    } else {
      return _stringMap;
    }
  }

  /// Get the mutex that guards the map for the given type
  template <typename T>
  std::mutex& getMutex() const {
    if constexpr (std::is_same_v<detail::GetVectorType<T>, int>) {
      return *(m_intMtx.get());
    } else if constexpr (std::is_same_v<detail::GetVectorType<T>, float>) {
      return *(m_floatMtx.get());
    } else {
      return *(m_stringMtx.get());
    }
  }

private:
  IntMap _intMap{};                                             ///< The map storing the integer values
  mutable MutexPtr m_intMtx{};    ///< The mutex guarding the integer map
  FloatMap _floatMap{};                                         ///< The map storing the float values
  mutable MutexPtr m_floatMtx{};  ///< The mutex guarding the float map
  StringMap _stringMap{};                                       ///< The map storing the double values
  mutable MutexPtr m_stringMtx{}; ///< The mutex guarding the float map
};

template <typename T, typename>
GenericDataReturnType<T> GenericParameters::getValue(const std::string& key) const {
  const auto& map = getMap<T>();
  auto& mtx = getMutex<T>();
  std::lock_guard lock{mtx};
  const auto it = map.find(key);
  // If there is no entry to the key, we just return an empty default
  // TODO: make this case detectable from the outside
  if (it == map.end()) {
    static const auto empty = T{};
    return empty;
  }

  // We have to check whether the return type is a vector or a single value
  if constexpr (detail::isVector<T>) {
    return it->second;
  } else {
    const auto& iv = it->second;
    return iv[0];
  }
}

template <typename T, typename>
void GenericParameters::setValue(const std::string& key, T value) {
  auto& map = getMap<T>();
  auto& mtx = getMutex<T>();

  if constexpr (detail::isVector<T>) {
    std::lock_guard lock{mtx};
    map.insert_or_assign(key, std::move(value));
  } else {
    // Wrap the value into a vector with exactly one entry and store that
    std::vector<T> v = {value};
    std::lock_guard lock{mtx};
    map.insert_or_assign(key, std::move(v));
  }
}

template <typename T, typename>
size_t GenericParameters::getN(const std::string& key) const {
  const auto& map = getMap<T>();
  auto& mtx = getMutex<T>();
  std::lock_guard lock{mtx};
  if (const auto it = map.find(key); it != map.end()) {
    return it->second.size();
  }
  return 0;
}

template <typename T, typename>
std::vector<std::string> GenericParameters::getKeys() const {
  std::vector<std::string> keys;
  const auto& map = getMap<T>();
  keys.reserve(map.size());
  {
    auto& mtx = getMutex<T>();
    std::lock_guard lock{mtx};
    std::transform(map.begin(), map.end(), std::back_inserter(keys), [](const auto& pair) { return pair.first; });
  }

  return keys;
}

} // namespace podio
#endif
