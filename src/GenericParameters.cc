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

void GenericParameters::print(std::ostream& os, bool flush) const {
  fmt::format_to(std::ostreambuf_iterator(os), "{}", *this);
  if (flush) {
    os.flush();
  }
}

} // namespace podio

fmt::format_context::iterator fmt::formatter<podio::GenericParameters>::format(const podio::GenericParameters& params,
                                                                               fmt::format_context& ctx) const {
  auto out = ctx.out();

  auto formatMap = [&out](const auto& map) {
    out = fmt::format_to(out, "{:<30}{}\n{:-<80}\n", "Key", "Value", "");
    for (const auto& [key, value] : map) {
      out = fmt::format_to(out, "{:<30}{}\n", key, value);
    }
  };

  out = fmt::format_to(out, "int parameters\n\n");
  formatMap(params.getMap<int>());
  out = fmt::format_to(out, "float parameters\n\n");
  formatMap(params.getMap<float>());
  out = fmt::format_to(out, "double parameters\n\n");
  formatMap(params.getMap<double>());
  out = fmt::format_to(out, "string parameters\n\n");
  formatMap(params.getMap<std::string>());

  return out;
}
