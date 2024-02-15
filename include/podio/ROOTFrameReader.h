#ifndef PODIO_ROOTFRAMEREADER_H
#define PODIO_ROOTFRAMEREADER_H

#include "podio/ROOTReader.h"

namespace podio {
using ROOTFrameReader [[deprecated("Will be removed in v1.0, switch to podio::ROOTReader")]] = podio::ROOTReader;
}

#endif // PODIO_ROOTFRAMEREADER_H
