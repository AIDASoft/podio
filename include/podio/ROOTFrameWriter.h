#ifndef PODIO_ROOTFRAMEWRITER_H
#define PODIO_ROOTFRAMEWRITER_H

#include "podio/ROOTWriter.h"

namespace podio {
using ROOTFrameWriter [[deprecated("Will be removed in v1.0, switch to podio::ROOTWriter")]] = podio::ROOTWriter;
}

#endif // PODIO_ROOTFRAMEWRITER_H
