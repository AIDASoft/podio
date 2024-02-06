#ifndef PODIO_SIOFRAMEWRITER_H
#define PODIO_SIOFRAMEWRITER_H

#include "podio/SIOWriter.h"

namespace podio {
using SIOFrameWriter [[deprecated("Will be removed in v1.0 switch podio::SIOWriter")]] = podio::SIOWriter;
}

#endif // PODIO_SIOFRAMEWRITER_H
