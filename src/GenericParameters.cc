#include "podio/GenericParameters.h"

#include <iomanip>

namespace podio {

GenericParameters::GenericParameters(const GenericParameters& other) {
  // acquire all locks at once to make sure all internal maps are
  // copied at the same "state" of the GenericParameters
  auto& intMtx = other.getMutex<int>();
  auto& floatMtx = other.getMutex<float>();
  auto& stringMtx = other.getMutex<std::string>();
  auto& doubleMtx = other.getMutex<double>();
  std::scoped_lock lock(intMtx, floatMtx, stringMtx, doubleMtx);
  _intMap = other._intMap;
  _floatMap = other._floatMap;
  _stringMap = other._stringMap;
  _doubleMap = other._doubleMap;
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
  const auto osflags = os.flags();
  os << std::left << std::setw(30) << "Key "
     << "Value " << '\n';
  os << "--------------------------------------------------------------------------------\n";
  for (const auto& [key, value] : map) {
    os << std::left << std::setw(30) << key << value << '\n';
  }

  os.flags(osflags);
}

void GenericParameters::print(std::ostream& os, bool flush) const {
  os << "int parameters\n\n";
  printMap(getMap<int>(), os);
  os << "\nfloat parameters\n";
  printMap(getMap<float>(), os);
  os << "\ndouble parameters\n";
  printMap(getMap<double>(), os);
  os << "\nstd::string parameters\n";
  printMap(getMap<std::string>(), os);

  if (flush) {
    os.flush();
  }
}

} // namespace podio
