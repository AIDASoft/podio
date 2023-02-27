#ifndef PODIO_SCHEMAEVOLUTION_H
#define PODIO_SCHEMAEVOLUTION_H

#include <cstdint>

namespace podio {

    enum class Backend { ROOT, SIO };

    using SchemaVersionT = uint32_t;

} // namespace podio

#endif