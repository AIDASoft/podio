#include "podio/SIOFrameWriter.h"
#include "podio/CollectionBase.h"
#include "podio/CollectionIDTable.h"
#include "podio/Frame.h"
#include "podio/GenericParameters.h"
#include "podio/SIOBlock.h"

#include <sio/api.h>
#include <sio/block.h>
#include <sio/compression/zlib.h>
#include <sio/exception.h>

#include <memory>

namespace podio {

namespace sio_utils {
  using StoreCollection = std::pair<const std::string&, const podio::CollectionBase*>;

  std::shared_ptr<SIOCollectionIDTableBlock> createCollIDBlock(const std::vector<StoreCollection>& collections,
                                                               const podio::CollectionIDTable& collIdTable) {
    std::vector<std::string> types;
    types.reserve(collections.size());
    std::vector<short> subsetColl;
    subsetColl.reserve(collections.size());

    for (const auto& [_, coll] : collections) {
      types.emplace_back(coll->getValueTypeName());
      subsetColl.emplace_back(coll->isSubsetCollection());
    }

    return std::make_shared<SIOCollectionIDTableBlock>(collIdTable.names(), collIdTable.ids(), std::move(types),
                                                       std::move(subsetColl));
  }

  sio::block_list createBlocks(const std::vector<StoreCollection>& collections,
                               const podio::GenericParameters& parameters) {
    sio::block_list blocks;
    blocks.reserve(collections.size() + 1); // parameters + collections

    auto paramBlock = std::make_shared<SIOEventMetaDataBlock>();
    // TODO: get rid of const_cast
    paramBlock->metadata = const_cast<podio::GenericParameters*>(&parameters);
    blocks.emplace_back(std::move(paramBlock));

    for (const auto& [name, col] : collections) {
      blocks.emplace_back(podio::SIOBlockFactory::instance().createBlock(col, name));
    }

    return blocks;
  }

  // Write the passed record and return where it starts in the file
  sio::ifstream::pos_type writeRecord(const sio::block_list& blocks, const std::string& recordName,
                                      sio::ofstream& stream, std::size_t initBufferSize = sio::mbyte,
                                      bool compress = true) {
    auto buffer = sio::buffer{initBufferSize};
    auto recInfo = sio::api::write_record(recordName, buffer, blocks, 0);

    if (compress) {
      // use zlib to compress the record into another buffer
      sio::zlib_compression compressor;
      compressor.set_level(6); // Z_DEFAULT_COMPRESSION==6
      auto comBuffer = sio::buffer{initBufferSize};
      sio::api::compress_record(recInfo, buffer, comBuffer, compressor);

      sio::api::write_record(stream, buffer.span(0, recInfo._header_length), comBuffer.span(), recInfo);
    } else {
      sio::api::write_record(stream, buffer.span(), recInfo);
    }

    return recInfo._file_start;
  }
} // namespace sio_utils

SIOFrameWriter::SIOFrameWriter(const std::string& filename) {
  m_stream.open(filename, std::ios::binary);
  if (!m_stream.is_open()) {
    SIO_THROW(sio::error_code::not_open, "Couldn't open output stream '" + filename + "'");
  }

  auto& libLoader [[maybe_unused]] = SIOBlockLibraryLoader::instance();

  sio::block_list blocks;
  blocks.emplace_back(std::make_shared<SIOVersionBlock>(podio::version::build_version));
  // write the version uncompressed
  sio_utils::writeRecord(blocks, "podio_header_info", m_stream, sizeof(podio::version::Version), false);
}

void SIOFrameWriter::writeFrame(const podio::Frame& frame, const std::string& category) {
  writeFrame(frame, category, frame.getAvailableCollections());
}

void SIOFrameWriter::writeFrame(const podio::Frame& frame, const std::string& category,
                                const std::vector<std::string>& collsToWrite) {
  std::vector<sio_utils::StoreCollection> collections;
  collections.reserve(collsToWrite.size());
  for (const auto& name : collsToWrite) {
    collections.emplace_back(name, frame.getCollectionForWrite(name));
  }

  // Write necessary metadata and the actual data into two different records.
  // Otherwise we cannot easily unpack the data record, because necessary
  // information is contained within the record.
  sio::block_list tableBlocks;
  tableBlocks.emplace_back(sio_utils::createCollIDBlock(collections, frame.getCollectionIDTableForWrite()));
  m_tocRecord.addRecord(category, sio_utils::writeRecord(tableBlocks, category + "_idTable", m_stream));

  auto blocks = sio_utils::createBlocks(collections, frame.getGenericParametersForWrite());
  sio_utils::writeRecord(blocks, category, m_stream);
}

void SIOFrameWriter::finish() {
  sio::block_list blocks;
  blocks.emplace_back(std::make_shared<SIOFileTOCRecordBlock>(&m_tocRecord));

  auto tocStartPos = sio_utils::writeRecord(blocks, sio_helpers::SIOTocRecordName, m_stream);

  // Now that we know the position of the TOC Record, put this information
  // into a final marker that can be identified and interpreted when reading
  // again
  uint64_t finalWords = (((uint64_t)sio_helpers::SIOTocMarker) << 32) | ((uint64_t)tocStartPos & 0xffffffff);
  m_stream.write(reinterpret_cast<char*>(&finalWords), sizeof(finalWords));

  m_stream.close();
}

} // namespace podio
