#ifndef PODIO_SIORAWDATA_H
#define PODIO_SIORAWDATA_H

#include "podio/CollectionBuffers.h"
#include "podio/CollectionIDTable.h"
#include "podio/GenericParameters.h"
#include "podio/SIOBlock.h"

#include "sio/buffer.h"
#include "sio/compression/zlib.h"
#include "sio/definitions.h"

#include <memory>
#include <numeric>
#include <optional>
#include <string>
#include <vector>

namespace podio {
class SIORawData {
  using CollIDPtr = std::shared_ptr<const podio::CollectionIDTable>;

public:
  SIORawData() = delete;
  ~SIORawData() = default;

  SIORawData(const SIORawData&) = delete;
  SIORawData& operator=(const SIORawData&) = delete;

  SIORawData(SIORawData&&) = default;
  SIORawData& operator=(SIORawData&&) = default;

  SIORawData(sio::buffer&& buffer, unsigned uncompressedSize, CollIDPtr&& idTable,
             const std::vector<std::string>& typeNames, const std::vector<short>& subsetCollBits) :
      m_recBuffer(std::move(buffer)),
      m_uncBuffer(uncompressedSize),
      m_typeNames(typeNames),
      m_subsetCollectionBits(subsetCollBits),
      m_idTable(idTable) {
  }

  std::optional<podio::CollectionReadBuffers> getCollectionBuffers(const std::string& name) {
    unpackBuffers();

    if (m_idTable->present(name)) {
      const auto index = m_idTable->collectionID(name);
      // index because collection indices start at 1!
      m_availableBlocks[index] = 1;
      return {dynamic_cast<podio::SIOBlock*>(m_blocks[index].get())->getBuffers()};
    }

    return std::nullopt;
  }

  podio::CollectionIDTable getIDTable() const {
    return {m_idTable->ids(), m_idTable->names()};
  }

  std::unique_ptr<podio::GenericParameters> getParameters() {
    unpackBuffers();
    m_availableBlocks[0] = 0;
    return std::make_unique<podio::GenericParameters>(std::move(m_parameters));
  }

  std::vector<std::string> getAvailableCollections() const {
    std::vector<std::string> collections;
    for (size_t i = 0; i < m_blocks.size(); ++i) {
      if (m_availableBlocks[i]) {
        collections.push_back(m_idTable->name(i));
      }
    }

    return collections;
  }

private:
  void unpackBuffers() {
    // Only do the unpacking once. Use the block as proxy for deciding whether
    // we have already unpacked things, since that is the main thing we do in
    // here: create blocks and read the data into them
    if (!m_blocks.empty()) {
      return;
    }

    createBlocks();

    sio::zlib_compression compressor;
    compressor.uncompress(m_recBuffer.span(), m_uncBuffer);
    sio::api::read_blocks(m_uncBuffer.span(), m_blocks);
  }

  void createBlocks() {
    m_blocks.reserve(m_typeNames.size() + 1);
    // First block during writing is metadata / parameters, so make sure to read
    // that first as well
    auto parameters = std::make_shared<podio::SIOEventMetaDataBlock>();
    parameters->metadata = &m_parameters;
    m_blocks.push_back(parameters);

    for (size_t i = 0; i < m_typeNames.size(); ++i) {
      const bool subsetColl = !m_subsetCollectionBits.empty() && m_subsetCollectionBits[i];
      auto blk = podio::SIOBlockFactory::instance().createBlock(m_typeNames[i], m_idTable->names()[i], subsetColl);
      m_blocks.push_back(blk);
    }

    m_availableBlocks.resize(m_blocks.size(), 1);
  }

  // sio::record_info m_rec_info{};
  // sio::buffer m_info_buffer{sio::max_record_info_len};

  // Default initialization doesn't really matter here, because they are made
  // the correct size on construction
  sio::buffer m_recBuffer{sio::mbyte};
  sio::buffer m_uncBuffer{sio::mbyte};

  std::vector<std::string> m_typeNames{};
  std::vector<short> m_subsetCollectionBits{};
  std::vector<short> m_availableBlocks{}; ///< The blocks that have already been retrieved

  sio::block_list m_blocks{};

  CollIDPtr m_idTable{nullptr};
  podio::GenericParameters m_parameters{};
};
} // namespace podio

#endif // PODIO_SIORAWDATA_H
