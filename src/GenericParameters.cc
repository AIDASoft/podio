#include "podio/GenericParameters.h"

#include <algorithm>

namespace podio {

int GenericParameters::getIntVal(const std::string& key) const {
  return getValue<int>(key);
}

float GenericParameters::getFloatVal(const std::string& key) const {
  return getValue<float>(key);
}

const std::string& GenericParameters::getStringVal(const std::string& key) const {
  return getValue<std::string>(key);
}

IntVec& GenericParameters::getIntVals(const std::string& key, IntVec& values) const {
  for (const auto v : getValue<std::vector<int>>(key)) {
    values.push_back(v);
  }
  return values;
}

FloatVec& GenericParameters::getFloatVals(const std::string& key, FloatVec& values) const {
  for (const auto v : getValue<std::vector<float>>(key)) {
    values.push_back(v);
  }
  return values;
}

StringVec& GenericParameters::getStringVals(const std::string& key, StringVec& values) const {
  for (const auto v : getValue<std::vector<std::string>>(key)) {
    values.push_back(v);
  }
  return values;
}

const StringVec& GenericParameters::getIntKeys(StringVec& keys) const {
  for (const auto& k : getKeys<int>()) {
    keys.push_back(k);
  }
  return keys;
}

const StringVec& GenericParameters::getFloatKeys(StringVec& keys) const {
  for (const auto& k : getKeys<float>()) {
    keys.push_back(k);
  }
  return keys;
}

const StringVec& GenericParameters::getStringKeys(StringVec& keys) const {
  for (const auto& k : getKeys<std::string>()) {
    keys.push_back(k);
  }
  return keys;
}

int GenericParameters::getNInt(const std::string& key) const {
  return getN<int>(key);
}

int GenericParameters::getNFloat(const std::string& key) const {
  return getN<float>(key);
}

int GenericParameters::getNString(const std::string& key) const {
  return getN<std::string>(key);
}

void GenericParameters::setValues(const std::string& key, const IntVec& values) {
  setValue(key, values);
}

void GenericParameters::setValues(const std::string& key, const FloatVec& values) {
  setValue(key, values);
}

void GenericParameters::setValues(const std::string& key, const StringVec& values) {
  setValue(key, values);
}

} // namespace podio
