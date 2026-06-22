#include "podio/utilities/ArrowTypeRegistry.h"

namespace podio {

ArrowTypeRegistry& ArrowTypeRegistry::mutInstance() {
  static ArrowTypeRegistry registry;
  return registry;
}

ArrowTypeRegistry const& ArrowTypeRegistry::instance() {
  return mutInstance();
}

void ArrowTypeRegistry::registerType(const std::string& typeName, std::shared_ptr<arrow::DataType> type) {
  m_registry[typeName] = std::move(type);
}

std::shared_ptr<arrow::DataType> ArrowTypeRegistry::getType(const std::string& typeName) const {
  auto it = m_registry.find(typeName);
  if (it != m_registry.end()) {
    return it->second;
  }
  return nullptr;
}

} // namespace podio
