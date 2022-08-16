#ifndef PODIO_UTILITIES_IOHELPERS_H
#define PODIO_UTILITIES_IOHELPERS_H

#ifndef PODIO_ENABLE_SIO
  #define PODIO_ENABLE_SIO 0
#endif

#include "podio/IReader.h"

#include <memory>
#include <string>

namespace podio {
std::unique_ptr<podio::IReader> createReader(const std::string& filename);
}

#endif
