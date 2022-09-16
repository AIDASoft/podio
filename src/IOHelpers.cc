#include "podio/utilities/IOHelpers.h"

#include "podio/ROOTReader.h"

#if PODIO_ENABLE_SIO
  #include "podio/SIOReader.h"
#endif

namespace podio {
std::unique_ptr<podio::IReader> createReader(const std::string& filename) {
  const auto fileEnding = [&filename]() -> std::string {
    const auto n = filename.rfind('.');
    if (n != std::string::npos) {
      return filename.substr(n);
    }
    return "";
  }();

  if (fileEnding.empty()) {
    return nullptr;
  }

  if (fileEnding == ".root") {
    return std::make_unique<podio::ROOTReader>();
  } else if (fileEnding == ".sio") {
#if PODIO_ENABLE_SIO
    return std::make_unique<podio::SIOReader>();
#else
    std::cerr << "PODIO: You are trying to open a .sio file but podio has not been built with SIO support\nMake sure "
                 "to build PODIO with SIO support to be able to read .sio files"
              << std::endl;
    return nullptr;
#endif
  } else {
    return nullptr;
  }
}

} // namespace podio
