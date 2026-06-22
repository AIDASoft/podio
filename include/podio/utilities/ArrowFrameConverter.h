#ifndef PODIO_ARROWFRAMECONVERTER_H
#define PODIO_ARROWFRAMECONVERTER_H

#include <memory>
#include <string>
#include <vector>

namespace arrow {
class DataType;
class Table;
}

namespace podio {

class Frame;

/**
 * @brief Get the standard Arrow DataType for object references/IDs.
 */
std::shared_ptr<arrow::DataType> objectRefType();

/**
 * @brief Convert a PODIO Frame to an Arrow Table (1-row).
 * 
 * Each collection specified in collsToWrite is mapped to a column of type List<Struct<...>>.
 * Metadata about the concrete collection value type is stored in each column field's KeyValueMetadata
 * under the key "value_type" (e.g. "value_type" -> "ExampleHit").
 * 
 * Frame parameters (podio::GenericParameters) are converted and included under the column "frame_parameters".
 * 
 * @param frame The Frame containing collections to convert.
 * @param collsToWrite The names of the collections to write to the Table.
 * @return A shared pointer to the created 1-row Arrow Table.
 */
std::shared_ptr<arrow::Table> convertFrameToTable(
    const podio::Frame& frame,
    const std::vector<std::string>& collsToWrite
);

} // namespace podio

#endif // PODIO_ARROWFRAMECONVERTER_H
