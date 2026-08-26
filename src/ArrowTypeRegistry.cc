#include "podio/utilities/ArrowTypeRegistry.h"
#include "podio/utilities/ArrowConverterRegistry.h"
#include <arrow/api.h>

namespace podio {

ArrowTypeRegistry::ArrowTypeRegistry() : m_registry() {
  m_registry["int"] = arrow::list(arrow::int32());
  m_registry["float"] = arrow::list(arrow::float32());
  m_registry["double"] = arrow::list(arrow::float64());
  m_registry["uint64_t"] = arrow::list(arrow::uint64());
  m_registry["uint32_t"] = arrow::list(arrow::uint32());
  m_registry["int64_t"] = arrow::list(arrow::int64());
  m_registry["int16_t"] = arrow::list(arrow::int16());
  m_registry["uint16_t"] = arrow::list(arrow::uint16());
  m_registry["int8_t"] = arrow::list(arrow::int8());
  m_registry["uint8_t"] = arrow::list(arrow::uint8());
}

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
  loadArrowLibraries();
  auto it = m_registry.find(typeName);
  if (it != m_registry.end()) {
    return it->second;
  }
  return nullptr;
}

} // namespace podio
