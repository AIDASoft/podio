#include "podio/utilities/ArrowConverterRegistry.h"
#include "podio/utilities/LibraryLoader.h"
#include <cstdlib>
#include <dlfcn.h>
#include <filesystem>
#include <iostream>
#include <sstream>

namespace podio {

ArrowConverterRegistry& ArrowConverterRegistry::mutInstance() {
  static ArrowConverterRegistry registry;
  return registry;
}

ArrowConverterRegistry const& ArrowConverterRegistry::instance() {
  return mutInstance();
}

void ArrowConverterRegistry::registerConverter(const std::string& typeName, CreatorFunc&& converter) {
  m_registry[typeName] = std::move(converter);
}

ArrowConverterRegistry::CreatorFunc ArrowConverterRegistry::getConverter(const std::string& typeName) const {
  loadArrowLibraries();
  auto it = m_registry.find(typeName);
  if (it != m_registry.end()) {
    return it->second;
  }
  return nullptr;
}

void ArrowConverterRegistry::registerReader(const std::string& typeName, BufferReaderFunc&& reader) {
  m_readerRegistry[typeName] = std::move(reader);
}

ArrowConverterRegistry::BufferReaderFunc ArrowConverterRegistry::getReader(const std::string& typeName) const {
  loadArrowLibraries();
  auto it = m_readerRegistry.find(typeName);
  if (it != m_readerRegistry.end()) {
    return it->second;
  }
  return nullptr;
}

void loadArrowLibraries() {
  static podio::utilities::LibraryLoader me("PODIO_ARROW_PATH", "Arrow", "Arrow");
}

} // namespace podio
