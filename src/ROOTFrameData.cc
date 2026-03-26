#include "podio/ROOTFrameData.h"

namespace podio {

ROOTFrameData::ROOTFrameData(BufferMap&& buffers, CollIDPtr&& idTable, podio::GenericParameters&& params) :
    m_buffers(std::move(buffers)), m_idTable(std::move(idTable)), m_parameters(std::move(params)) {
}

ROOTFrameData::ROOTFrameData(BufferMap&& buffers, CollIDPtr&& idTable, podio::GenericParameters&& params,
                             LazyReadFn&& lazyRead) :
    m_buffers(std::move(buffers)),
    m_idTable(std::move(idTable)),
    m_parameters(std::move(params)),
    m_lazyRead(std::move(lazyRead)) {
}

std::optional<podio::CollectionReadBuffers> ROOTFrameData::getCollectionBuffers(const std::string& name) {
  auto bufferHandle = m_buffers.extract(name);
  if (!bufferHandle.empty()) {
    return {std::move(bufferHandle.mapped())};
  }
  if (m_lazyRead) {
    return m_lazyRead(name);
  }
  return std::nullopt;
}

podio::CollectionIDTable ROOTFrameData::getIDTable() const {
  // Construct a copy of the internal table
  return {m_idTable->ids(), m_idTable->names()};
}

std::unique_ptr<podio::GenericParameters> ROOTFrameData::getParameters() {
  return std::make_unique<podio::GenericParameters>(std::move(m_parameters));
}

std::vector<std::string> ROOTFrameData::getAvailableCollections() const {
  std::vector<std::string> collections;
  collections.reserve(m_buffers.size());
  for (const auto& [name, _] : m_buffers) {
    collections.push_back(name);
  }

  return collections;
}

} // namespace podio
