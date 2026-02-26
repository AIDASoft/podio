#ifndef PODIO_TRANSPORTFRAMEDATA_H
#define PODIO_TRANSPORTFRAMEDATA_H

#include "podio/CollectionBuffers.h"
#include "podio/CollectionIDTable.h"
#include "podio/GenericParameters.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace podio {

/// Minimal draft backend for exploring frame-level transport at FrameData layer.
///
/// This intentionally provides read-side stubs only and does not perform any
/// serialization or deserialization.
class TransportFrameData {
public:
  TransportFrameData() = default;
  ~TransportFrameData() = default;

  TransportFrameData(const TransportFrameData&) = delete;
  TransportFrameData& operator=(const TransportFrameData&) = delete;
  TransportFrameData(TransportFrameData&&) = default;
  TransportFrameData& operator=(TransportFrameData&&) = default;

  TransportFrameData(podio::CollectionIDTable idTable, std::vector<std::string> availableCollections,
                     podio::GenericParameters parameters);

  std::optional<podio::CollectionReadBuffers> getCollectionBuffers(const std::string& name);

  podio::CollectionIDTable getIDTable() const;

  std::vector<std::string> getAvailableCollections() const;

  std::unique_ptr<podio::GenericParameters> getParameters();

private:
  podio::CollectionIDTable m_idTable{};
  std::vector<std::string> m_availableCollections{};
  podio::GenericParameters m_parameters{};
};

} // namespace podio

#endif // PODIO_TRANSPORTFRAMEDATA_H
