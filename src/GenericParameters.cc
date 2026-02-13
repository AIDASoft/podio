#include "podio/GenericParameters.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

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

template <typename MapType>
void formatMap(const MapType& map, std::ostream& os) {
  fmt::format_to(std::ostreambuf_iterator(os), "{:<30}{}\n{:-<80}", "Key", "Value", "");
  for (const auto& [key, value] : map) {
    fmt::format_to(std::ostreambuf_iterator(os), "{:<30}{}\n", key, value);
  }
}

void GenericParameters::print(std::ostream& os, bool flush) const {
  os << "int parameters\n\n";
  formatMap(getMap<int>(), os);
  os << "float parameters\n\n";
  formatMap(getMap<float>(), os);
  os << "double parameters\n\n";
  formatMap(getMap<double>(), os);
  os << "string parameters\n\n";
  formatMap(getMap<std::string>(), os);

  if (flush) {
    os.flush();
  }
}

} // namespace podio
