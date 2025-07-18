#ifndef PODIO_SIOFRAMEDATA_H
#define PODIO_SIOFRAMEDATA_H

#include "podio/CollectionBuffers.h"
#include "podio/CollectionIDTable.h"
#include "podio/GenericParameters.h"

#include <sio/buffer.h>
#include <sio/definitions.h>

#include <optional>
#include <string>
#include <vector>

namespace podio {
/// The Frame data container for the SIO backend. It is constructed from the
/// compressed sio::buffers that is read from file and does all the necessary
/// unpacking and decompressing internally after construction.
class SIOFrameData {

public:
  SIOFrameData() = delete;
  ~SIOFrameData();

  SIOFrameData(const SIOFrameData&) = delete;
  SIOFrameData& operator=(const SIOFrameData&) = delete;

  SIOFrameData(SIOFrameData&&) = default;
  SIOFrameData& operator=(SIOFrameData&&) = default;

  /// Constructor from the collBuffers containing the collection data and a
  /// tableBuffer containing the necessary information for unpacking the
  /// collections. The two size parameters denote the uncompressed size of the
  /// respective buffers.
  ///
  /// In case the limitColls contain a collection name that is not available
  /// from the idTable names this throws an exception
  SIOFrameData(sio::buffer&& collBuffers, std::size_t dataSize, sio::buffer&& tableBuffer, std::size_t tableSize,
               std::vector<std::string> limitColls = {});

  std::optional<podio::CollectionReadBuffers> getCollectionBuffers(const std::string& name);

  podio::CollectionIDTable getIDTable() const {
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

  /// The collections that should be made available for a Frame constructed from
  /// this (if non-empty)
  std::vector<std::string> m_limitColls{};
};
} // namespace podio

#endif // PODIO_SIOFRAMEDATA_H
