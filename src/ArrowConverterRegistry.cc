#include "podio/utilities/ArrowConverterRegistry.h"
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
  ArrowConverterLibraryLoader::instance();
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
  ArrowConverterLibraryLoader::instance();
  auto it = m_readerRegistry.find(typeName);
  if (it != m_readerRegistry.end()) {
    return it->second;
  }
  return nullptr;
}

ArrowConverterLibraryLoader::ArrowConverterLibraryLoader() {
  for (const auto& [lib, dir] : getLibNames()) {
    const auto status = loadLib(lib, dir);
    switch (status) {
    case LoadStatus::Success:
      std::cerr << "Loaded Arrow library \'" << lib << "\' (from " << dir << ")" << std::endl;
      break;
    case LoadStatus::AlreadyLoaded:
      std::cerr << "Arrow library \'" << lib << "\' already loaded. Not loading again from " << dir << std::endl;
      break;
    case LoadStatus::Error: {
      const char* err = dlerror();
      std::cerr << "ERROR while loading Arrow library \'" << lib << "\' (from " << dir
                << "): " << (err ? err : "Unknown error") << std::endl;
      break;
    }
    }
  }
}

ArrowConverterLibraryLoader::LoadStatus ArrowConverterLibraryLoader::loadLib(const std::string& libname,
                                                                             const std::string& directory) {
  if (m_loadedLibs.find(libname) != m_loadedLibs.end()) {
    return LoadStatus::AlreadyLoaded;
  }
  void* libhandle = dlopen((directory + "/" + libname).c_str(), RTLD_LAZY | RTLD_GLOBAL);
  if (libhandle) {
    m_loadedLibs.insert({libname, libhandle});
    return LoadStatus::Success;
  }

  return LoadStatus::Error;
}

std::vector<std::tuple<std::string, std::string>> ArrowConverterLibraryLoader::getLibNames() {
  namespace fs = std::filesystem;
  std::vector<std::tuple<std::string, std::string>> libs;

  const auto ldLibPath = []() {
    auto pathVar = std::getenv("PODIO_ARROW_PATH");
    if (!pathVar) {
      pathVar = std::getenv("LD_LIBRARY_PATH");
    }
    return pathVar;
  }();
  if (!ldLibPath) {
    return libs;
  }

  std::string dir;
  std::istringstream stream(ldLibPath);
  while (std::getline(stream, dir, ':')) {
    if (not fs::exists(dir)) {
      continue;
    }

    for (auto& lib : fs::directory_iterator(dir)) {
      const auto filename = lib.path().filename().string();
      if (filename.find("Arrow") != std::string::npos) {
        libs.emplace_back(std::move(filename), dir);
      }
    }
  }

  return libs;
}

} // namespace podio
