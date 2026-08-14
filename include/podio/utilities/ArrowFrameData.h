#ifndef PODIO_ARROWFRAMEDATA_H
#define PODIO_ARROWFRAMEDATA_H

#include "podio/CollectionBuffers.h"
#include "podio/CollectionIDTable.h"
#include "podio/GenericParameters.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace arrow {
class Table;
} // namespace arrow

namespace podio {

class ArrowFrameData {
public:
  ArrowFrameData(std::shared_ptr<arrow::Table> table, int64_t rowIndex,
                 const std::vector<std::string>& collsToRead = {});

  podio::CollectionIDTable getIDTable() const;
  std::optional<podio::CollectionReadBuffers> getCollectionBuffers(const std::string& name);
  // Must return by value to satisfy the C++20 FrameDataType concept (as other backends construct it dynamically)
  std::vector<std::string> getAvailableCollections() const;
  std::unique_ptr<podio::GenericParameters> getParameters();

private:
  std::shared_ptr<arrow::Table> m_table;
  int64_t m_rowIndex;
  std::vector<std::string> m_availableCollections;
  podio::CollectionIDTable m_idTable;
};

} // namespace podio

#endif // PODIO_ARROWFRAMEDATA_H
