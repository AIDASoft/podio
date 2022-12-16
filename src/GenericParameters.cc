#include "podio/GenericParameters.h"

#include <algorithm>
#include <iomanip>

namespace podio {

GenericParameters::GenericParameters() :
    m_intMtx(std::make_unique<std::mutex>()),
    m_floatMtx(std::make_unique<std::mutex>()),
    m_stringMtx(std::make_unique<std::mutex>()) {
}

GenericParameters::GenericParameters(const GenericParameters& other) :
    m_intMtx(std::make_unique<std::mutex>()),
    m_floatMtx(std::make_unique<std::mutex>()),
    m_stringMtx(std::make_unique<std::mutex>()) {
  {
    // acquire all three locks at once to make sure all three internal maps are
    // copied at the same "state" of the GenericParameters
    auto& intMtx = other.getMutex<int>();
    auto& floatMtx = other.getMutex<float>();
    auto& stringMtx = other.getMutex<std::string>();
    std::scoped_lock lock(intMtx, floatMtx, stringMtx);
    _intMap = other._intMap;
    _floatMap = other._floatMap;
    _stringMap = other._stringMap;
  }
}

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
  for (const auto& v : getValue<std::vector<std::string>>(key)) {
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

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& values) {
  os << "[";
  if (!values.empty()) {
    os << values[0];
    for (size_t i = 1; i < values.size(); ++i) {
      os << ", " << values[i];
    }
  }

  return os << "]";
}

template <typename MapType>
void printMap(const MapType& map, std::ostream& os) {
  os << std::left << std::setw(30) << "Key "
     << "Value " << '\n';
  os << "--------------------------------------------------------------------------------\n";
  for (const auto& [key, value] : map) {
    os << std::left << std::setw(30) << key << value << '\n';
  }
}

void GenericParameters::print(std::ostream& os, bool flush) {
  os << "int parameters\n\n";
  printMap(getMap<int>(), os);
  os << "\nfloat parameters\n";
  printMap(getMap<float>(), os);
  os << "\nstd::string parameters\n";
  printMap(getMap<std::string>(), os);

  if (flush) {
    os.flush();
  }
}

} // namespace podio
