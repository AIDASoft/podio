#include "podio/utilities/ArrowConverterRegistry.h"

namespace podio {

ArrowConverterRegistry& ArrowConverterRegistry::mutInstance() {
  static ArrowConverterRegistry registry;
  return registry;
}

ArrowConverterRegistry const& ArrowConverterRegistry::instance() {
  return mutInstance();
}

void ArrowConverterRegistry::registerConverter(const std::string& typeName, CreatorFunc converter) {
  m_registry[typeName] = std::move(converter);
}

ArrowConverterRegistry::CreatorFunc ArrowConverterRegistry::getConverter(const std::string& typeName) const {
  auto it = m_registry.find(typeName);
  if (it != m_registry.end()) {
    return it->second;
  }
  return nullptr;
}

} // namespace podio
