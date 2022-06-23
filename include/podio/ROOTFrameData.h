#ifndef PODIO_ROOTFRAMEDATA_H
#define PODIO_ROOTFRAMEDATA_H

#include "podio/CollectionBuffers.h"
#include "podio/CollectionIDTable.h"
#include "podio/GenericParameters.h"

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

namespace podio {

class ROOTFrameData {
  using CollIDPtr = std::shared_ptr<const podio::CollectionIDTable>;

public:
  using BufferMap = std::unordered_map<std::string, podio::CollectionReadBuffers>;

  ROOTFrameData() = delete;
  ~ROOTFrameData() = default;
  ROOTFrameData(ROOTFrameData&&) = default;
  ROOTFrameData& operator=(ROOTFrameData&&) = default;
  ROOTFrameData(const ROOTFrameData&) = delete;
  ROOTFrameData& operator=(const ROOTFrameData&) = delete;

  ROOTFrameData(BufferMap&& buffers, CollIDPtr&& idTable, podio::GenericParameters&& params) :
      m_buffers(std::move(buffers)), m_idTable(idTable), m_parameters(std::move(params)) {
  }

  std::optional<podio::CollectionReadBuffers> getCollectionBuffers(const std::string& name) {
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

  std::vector<std::string> getAvailableCollections() const {
    std::vector<std::string> collections;
    collections.reserve(m_buffers.size());
    for (const auto& [name, _] : m_buffers) {
      collections.push_back(name);
    }

    return collections;
  }

private:
  // TODO: switch to something more elegant once the basic functionality and
  // interface is better defined
  BufferMap m_buffers{};
  // This is co-owned by each FrameData and the original reader. (for now at least)
  CollIDPtr m_idTable{nullptr};
  podio::GenericParameters m_parameters{};
};
} // namespace podio

#endif // PODIO_ROOTFRAMEDATA_H
