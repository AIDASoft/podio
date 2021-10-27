#ifndef PODIO_UTILITIES_H
#define PODIO_UTILITIES_H

#include "podio/IReader.h"
#include "podio/ROOTReader.h"

#ifndef PODIO_ENABLE_SIO
#define PODIO_ENABLE_SIO 0
#endif
#if PODIO_ENABLE_SIO
#include "podio/SIOReader.h"
#endif

#include <memory>
#include <iostream>
#include <array>

namespace podio::utils {
std::unique_ptr<podio::IReader> createReader(const std::string& FileName) {
  const auto fileEnding = [&FileName]() -> std::string {
    const auto n = FileName.rfind('.');
    if (n != std::string::npos) {
      return FileName.substr(n);
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
    std::cerr << "PODIO: You are trying to open a .sio file but podio has not been built with SIO support\nMake sure to build PODIO with SIO support to be able to read .sio files" << std::endl;
    return nullptr;
#endif
  } else {
    return nullptr;
  }
}

}


#endif
