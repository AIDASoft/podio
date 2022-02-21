// -*- C++ -*-
#ifndef PODIO_GENERICPARAMETERS_H
#define PODIO_GENERICPARAMETERS_H 1

#include "podio/utilities/TypeHelpers.h"

#include <algorithm>
#include <map>
#include <string>
#include <vector>

namespace podio {

/**
 * The types which are supported in the GenericParameters
 */
using SupportedGenericDataTypes = std::tuple<int, float, std::string>;

template <typename T>
static constexpr bool isSupportedGenericDataType = detail::isAnyOrVectorOf<T, SupportedGenericDataTypes>;

/**
 * Alias template to be used for enabling / disabling template overloads that
 * should only be present for actually supported data types
 */
template <typename T>
using EnableIfValidGenericDataType = typename std::enable_if_t<isSupportedGenericDataType<T>>;

/**
 * The data types for which a return by value should be preferred over a return
 * by const-ref
 */
using ValueReturnGenericDataTypes = std::tuple<int, float>;

/**
 * Alias template for enabling return by value overloads
 */
template <typename T>
using EnableIfValueReturnGenericDataType = std::enable_if_t<detail::isInTuple<T, ValueReturnGenericDataTypes>>;

/**
 * Alias template for return by const ref overloads
 */
template <typename T>
using EnableIfConstRefReturnGenericDataType =
    std::enable_if_t<std::is_same_v<T, std::string> ||
                     (detail::isInTuple<T, detail::TupleOfVector<SupportedGenericDataTypes>>)>;

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

public:
  /**
   * Get the value that is stored under the given key
   */
  template <typename T, typename = EnableIfConstRefReturnGenericDataType<T>>
  const T& getValue(const std::string& key) const;

  /**
   * Get the value that is stored under the given key
   */
  template <typename T, typename = EnableIfValueReturnGenericDataType<T>>
  T getValue(const std::string& key) const;

  template <typename T, typename = std::enable_if_t<isSupportedGenericDataType<T>>>
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

  template <typename T, typename = EnableIfValidGenericDataType<T>>
  size_t getN(const std::string& key) const;

  template <typename T, typename = EnableIfValidGenericDataType<T>>
  std::vector<std::string> getKeys() const;

  /** Returns the first integer value for the given key.
   */
  int getIntVal(const std::string& key) const;

  /** Returns the first float value for the given key.
   */
  float getFloatVal(const std::string& key) const;

  /** Returns the first string value for the given key.
   */
  const std::string& getStringVal(const std::string& key) const;

  /** Adds all integer values for the given key to values.
   *  Returns a reference to values for convenience.
   */
  IntVec& getIntVals(const std::string& key, IntVec& values) const;

  /** Adds all float values for the given key to values.
   *  Returns a reference to values for convenience.
   */
  FloatVec& getFloatVals(const std::string& key, FloatVec& values) const;

  /** Adds all float values for the given key to values.
   *  Returns a reference to values for convenience.
   */
  StringVec& getStringVals(const std::string& key, StringVec& values) const;

  /** Returns a list of all keys of integer parameters.
   */
  const StringVec& getIntKeys(StringVec& keys) const;

  /** Returns a list of all keys of float parameters.
   */
  const StringVec& getFloatKeys(StringVec& keys) const;

  /** Returns a list of all keys of string parameters.
   */
  const StringVec& getStringKeys(StringVec& keys) const;

  /** The number of integer values stored for this key.
   */
  int getNInt(const std::string& key) const;

  /** The number of float values stored for this key.
   */
  int getNFloat(const std::string& key) const;

  /** The number of string values stored for this key.
   */
  int getNString(const std::string& key) const;

  /** Set integer values for the given key.
   */
  void setValues(const std::string& key, const IntVec& values);

  /** Set float values for the given key.
   */
  void setValues(const std::string& key, const FloatVec& values);

  /** Set string values for the given key.
   */
  void setValues(const std::string& key, const StringVec& values);

  /// erase all elements
  void clear() {
    _intMap.clear();
    _floatMap.clear();
    _stringMap.clear();
  }

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
  template <typename T>
  const MapType<T>& getMap() const {
    if constexpr (std::is_same_v<T, int>) {
      return _intMap;
    } else if constexpr (std::is_same_v<T, float>) {
      return _floatMap;
    } else {
      return _stringMap;
    }
  }

  template <typename T>
  MapType<T>& getMap() {
    if constexpr (std::is_same_v<T, int>) {
      return _intMap;
    } else if constexpr (std::is_same_v<T, float>) {
      return _floatMap;
    } else {
      return _stringMap;
    }
  }

private:
  IntMap _intMap{};
  FloatMap _floatMap{};
  StringMap _stringMap{};

}; // class

template <typename T, typename>
const T& GenericParameters::getValue(const std::string& key) const {
  const auto& map = getMap<detail::GetVectorType<T>>();
  const auto it = map.find(key);
  // If there is no entry to the key, we just return an empty default
  // TODO: make this case detectable from the outside
  if (it == map.end()) {
    static const auto empty = T{};
    return empty;
  }

  // Here we have to check whether the return type is a vector or a single value
  if constexpr (detail::isVector<T>) {
    return it->second;
  } else {
    const auto& iv = it->second;
    return iv[0];
  }
}

template <typename T, typename>
T GenericParameters::getValue(const std::string& key) const {
  // Function not enabled for vector return types
  const auto& map = getMap<T>();
  const auto it = map.find(key);
  if (it == map.end()) {
    return T{};
  }
  // No need to differentiate between vector and single value return case here
  return (it->second)[0];
}

template <typename T, typename>
void GenericParameters::setValue(const std::string& key, T value) {
  auto& map = getMap<detail::GetVectorType<T>>();
  if constexpr (detail::isVector<T>) {
    map.insert_or_assign(key, std::move(value));
  } else {
    std::vector<detail::GetVectorType<T>> v = {value};
    map.insert_or_assign(key, std::move(v));
  }
}

template <typename T, typename>
size_t GenericParameters::getN(const std::string& key) const {
  const auto& map = getMap<detail::GetVectorType<T>>();
  if (const auto it = map.find(key); it != map.end()) {
    return it->second.size();
  }
  return 0;
}

template <typename T, typename>
std::vector<std::string> GenericParameters::getKeys() const {
  std::vector<std::string> keys;
  const auto& map = getMap<detail::GetVectorType<T>>();
  std::transform(map.begin(), map.end(), std::back_inserter(keys), [](const auto& pair) { return pair.first; });

  return keys;
}

} // namespace podio
#endif
