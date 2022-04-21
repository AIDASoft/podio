#ifndef PODIO_ROOTRAWDATA_H
#define PODIO_ROOTRAWDATA_H

#include "podio/CollectionBuffers.h"
#include "podio/CollectionIDTable.h"
#include "podio/GenericParameters.h"

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

namespace podio {

class ROOTRawData {
  using BufferMap = std::unordered_map<std::string, podio::CollectionBuffers>;
  using CollIDPtr = std::shared_ptr<const podio::CollectionIDTable>;

public:
  ROOTRawData() = delete;
  ~ROOTRawData() = default;
  ROOTRawData(ROOTRawData&&) = default;
  ROOTRawData& operator=(ROOTRawData&&) = default;
  ROOTRawData(const ROOTRawData&) = delete;
  ROOTRawData& operator=(const ROOTRawData&) = delete;

  ROOTRawData(BufferMap&& buffers, CollIDPtr&& idTable, podio::GenericParameters&& params) :
      m_buffers(std::move(buffers)), m_idTable(idTable), m_parameters(std::move(params)) {
  }

  std::optional<podio::CollectionBuffers> getCollectionBuffers(const std::string& name) {
    const auto bufferHandle = m_buffers.extract(name);
    if (bufferHandle.empty()) {
      return std::nullopt;
    }

    return {bufferHandle.mapped()};
  }

  podio::CollectionIDTable getIDTable() const {
    // Construct a copy of the internal table
    return {m_idTable->ids(), m_idTable->names()};
  }

  std::unique_ptr<podio::GenericParameters> getParameters() {
    return std::make_unique<podio::GenericParameters>(std::move(m_parameters));
  }

private:
  // TODO: switch to something more elegant once the basic functionality and
  // interface is better defined
  BufferMap m_buffers{};
  // This is co-owned by each rawdata and the original reader. (for now at least)
  CollIDPtr m_idTable{nullptr};
  podio::GenericParameters m_parameters{};
};
} // namespace podio

#endif // PODIO_ROOTRAWDATA_H
