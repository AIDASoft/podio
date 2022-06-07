#ifndef PODIO_SIORAWDATA_H
#define PODIO_SIORAWDATA_H

#include "podio/CollectionBuffers.h"
#include "podio/CollectionIDTable.h"
#include "podio/GenericParameters.h"
#include "podio/SIOBlock.h"

#include <sio/buffer.h>
#include <sio/definitions.h>

#include <memory>
#include <numeric>
#include <optional>
#include <string>
#include <vector>

namespace podio {
class SIORawData {
  // using CollIDPtr = std::shared_ptr<const podio::CollectionIDTable>;

public:
  SIORawData() = delete;
  ~SIORawData() = default;

  SIORawData(const SIORawData&) = delete;
  SIORawData& operator=(const SIORawData&) = delete;

  SIORawData(SIORawData&&) = default;
  SIORawData& operator=(SIORawData&&) = default;

  SIORawData(sio::buffer&& collBuffers, std::size_t dataSize, sio::buffer&& tableBuffer, std::size_t tableSize) :
      m_recBuffer(std::move(collBuffers)),
      m_tableBuffer(std::move(tableBuffer)),
      m_dataSize(dataSize),
      m_tableSize(tableSize) {
  }

  std::optional<podio::CollectionReadBuffers> getCollectionBuffers(const std::string& name);

  podio::CollectionIDTable getIDTable() {
    if (m_idTable.empty()) {
      readIdTable();
    }
    return {m_idTable.ids(), m_idTable.names()};
  }

  std::unique_ptr<podio::GenericParameters> getParameters();

  std::vector<std::string> getAvailableCollections();

private:
  void unpackBuffers();

  void readIdTable();

  void createBlocks();

  // Default initialization doesn't really matter here, because they are made
  // the correct size on construction
  sio::buffer m_recBuffer{sio::kbyte};   ///< The compressed record (data) buffer
  sio::buffer m_tableBuffer{sio::kbyte}; ///< The compressed collection id table buffer

  std::size_t m_dataSize{};  ///< Uncompressed data buffer size
  std::size_t m_tableSize{}; ///< Uncompressed table size

  std::vector<short> m_availableBlocks{}; ///< The blocks that have already been retrieved

  sio::block_list m_blocks{};

  podio::CollectionIDTable m_idTable{};
  std::vector<std::string> m_typeNames{};
  std::vector<short> m_subsetCollectionBits{};

  podio::GenericParameters m_parameters{};
};
} // namespace podio

#endif // PODIO_SIORAWDATA_H
