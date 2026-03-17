#include "podio/TransportFrameData.h"

#include <utility>

namespace podio {

TransportFrameData::TransportFrameData(podio::CollectionIDTable idTable, std::vector<std::string> availableCollections,
                                       podio::GenericParameters parameters) :
    m_idTable(std::move(idTable)),
    m_availableCollections(std::move(availableCollections)),
    m_parameters(std::move(parameters)) {
}

std::optional<podio::CollectionReadBuffers> TransportFrameData::getCollectionBuffers(const std::string& /*name*/) {
  return std::nullopt;
}

podio::CollectionIDTable TransportFrameData::getIDTable() const {
  return {m_idTable.ids(), m_idTable.names()};
}

std::vector<std::string> TransportFrameData::getAvailableCollections() const {
  return m_availableCollections;
}

std::unique_ptr<podio::GenericParameters> TransportFrameData::getParameters() {
  return std::make_unique<podio::GenericParameters>(m_parameters);
}

} // namespace podio
