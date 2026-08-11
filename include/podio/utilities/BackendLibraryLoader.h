#ifndef PODIO_UTILITIES_BACKENDLIBRARYLOADER_H
#define PODIO_UTILITIES_BACKENDLIBRARYLOADER_H

#include <map>
#include <string>
#include <tuple>
#include <vector>

namespace podio {
namespace utilities {

  class BackendLibraryLoader {
  public:
    enum class LoadStatus : short { Success = 0, AlreadyLoaded = 1, Error = 2 };

    BackendLibraryLoader(std::string envVarName, std::string libraryPattern, std::string logDesignator);
    ~BackendLibraryLoader() = default;

  private:
    LoadStatus loadLib(const std::string& libname, const std::string& directory);
    std::vector<std::tuple<std::string, std::string>> getLibNames() const;

    std::string m_envVarName;
    std::string m_libraryPattern;
    std::string m_logDesignator;
    std::map<std::string, void*> m_loadedLibs{};
  };

} // namespace utilities
} // namespace podio

#endif // PODIO_UTILITIES_BACKENDLIBRARYLOADER_H
