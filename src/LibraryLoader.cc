#include "podio/utilities/LibraryLoader.h"
#include <cstdlib>
#include <dlfcn.h>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace podio {
namespace utilities {

  LibraryLoader::LibraryLoader(std::string envVarName, std::string libraryPattern, std::string logDesignator) :
      m_envVarName(std::move(envVarName)),
      m_libraryPattern(std::move(libraryPattern)),
      m_logDesignator(std::move(logDesignator)) {

    for (const auto& [lib, dir] : getLibNames()) {
      const auto status = loadLib(lib, dir);
      switch (status) {
      case LoadStatus::Success:
        std::cerr << "Loaded " << m_logDesignator << " library \'" << lib << "\' (from " << dir << ")" << std::endl;
        break;
      case LoadStatus::AlreadyLoaded:
        std::cerr << m_logDesignator << " library \'" << lib << "\' already loaded. Not loading again from " << dir
                  << std::endl;
        break;
      case LoadStatus::Error: {
        const char* err = dlerror();
        std::cerr << "ERROR while loading " << m_logDesignator << " library \'" << lib << "\' (from " << dir
                  << "): " << (err ? err : "Unknown error") << std::endl;
        break;
      }
      }
    }
  }

  LibraryLoader::LoadStatus LibraryLoader::loadLib(const std::string& libname, const std::string& directory) {
    if (m_loadedLibs.find(libname) != m_loadedLibs.end()) {
      return LoadStatus::AlreadyLoaded;
    }
    dlerror(); // Clear any existing error
    void* libhandle = dlopen((directory + "/" + libname).c_str(), RTLD_LAZY | RTLD_GLOBAL);
    if (libhandle) {
      m_loadedLibs.insert({libname, libhandle});
      return LoadStatus::Success;
    }

    return LoadStatus::Error;
  }

  std::vector<std::tuple<std::string, std::string>> LibraryLoader::getLibNames() const {
    namespace fs = std::filesystem;
    std::vector<std::tuple<std::string, std::string>> libs;

    const auto ldLibPath = [this]() {
      auto pathVar = std::getenv(m_envVarName.c_str());
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
        if (filename.find(m_libraryPattern) != std::string::npos) {
          libs.emplace_back(std::move(filename), dir);
        }
      }

      if (std::getenv(m_envVarName.c_str()) && libs.empty()) {
        throw std::runtime_error("No " + m_logDesignator + " libraries found in " + m_envVarName + ". Please set " +
                                 m_envVarName + " to the directory containing the " + m_logDesignator +
                                 " libraries or unset it to fallback to LD_LIBRARY_PATH.");
      }
    }

    return libs;
  }

} // namespace utilities
} // namespace podio
