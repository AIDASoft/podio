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
  ArrowFrameData(std::shared_ptr<arrow::Table> table, int64_t rowIndex);

  podio::CollectionIDTable getIDTable() const;
  std::optional<podio::CollectionReadBuffers> getCollectionBuffers(const std::string& name);
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
