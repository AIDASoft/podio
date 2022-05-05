#include "podio/SIOFrameReader.h"
#include "podio/SIOBlock.h"

#include "sio/compression/zlib.h"
#include <sio/definitions.h>

namespace podio {

SIOFrameReader::SIOFrameReader() {
  auto& libLoader [[maybe_unused]] = SIOBlockLibraryLoader::instance();
}

void SIOFrameReader::openFile(const std::string& filename) {
  m_stream.open(filename, std::ios::binary);
  readCollectionIDTable();
  readFileTOCRecord();
}

std::unique_ptr<SIORawData> SIOFrameReader::readNextEvent() {
  // skip possible intermediate records that are not event data
  sio::api::go_to_record(m_stream, "event_record");

  sio::record_info recInfo;
  sio::buffer infoBuffer{sio::max_record_info_len};
  sio::buffer recBuffer{sio::mbyte}; // correct size?
  sio::api::read_record_info(m_stream, recInfo, infoBuffer);
  sio::api::read_record_data(m_stream, recInfo, recBuffer);

  m_eventNumber++;
  return std::make_unique<SIORawData>(std::move(recBuffer), recInfo._uncompressed_length, m_table, m_typeNames,
                                      m_subsetCollectionBits);
}

void SIOFrameReader::readCollectionIDTable() {
  sio::buffer infoBuffer{sio::max_record_info_len};
  sio::buffer recBuffer{sio::mbyte};
  sio::record_info recInfo;
  sio::api::read_record_info(m_stream, recInfo, infoBuffer);
  sio::api::read_record_data(m_stream, recInfo, recBuffer);

  sio::buffer uncBuffer{sio::mbyte};
  uncBuffer.resize(recInfo._uncompressed_length);
  sio::zlib_compression compressor;
  compressor.uncompress(recBuffer.span(), uncBuffer);

  sio::block_list blocks;
  blocks.emplace_back(std::make_shared<SIOCollectionIDTableBlock>());
  blocks.emplace_back(std::make_shared<SIOVersionBlock>());
  sio::api::read_blocks(uncBuffer.span(), blocks);

  auto* idTableBlock = static_cast<SIOCollectionIDTableBlock*>(blocks[0].get());
  m_table = std::shared_ptr<podio::CollectionIDTable>(idTableBlock->getTable());
  m_typeNames = idTableBlock->getTypeNames();
  m_subsetCollectionBits = idTableBlock->getSubsetCollectionBits();
  m_fileVersion = static_cast<SIOVersionBlock*>(blocks[1].get())->version;
}

bool SIOFrameReader::readFileTOCRecord() {
  // Check if there is a dedicated marker at the end of the file that tells us
  // where the TOC actually starts
  m_stream.seekg(-sio_helpers::SIOTocInfoSize, std::ios_base::end);
  uint64_t firstWords{0};
  m_stream.read(reinterpret_cast<char*>(&firstWords), sizeof(firstWords));

  const uint32_t marker = (firstWords >> 32) & 0xffffffff;
  if (marker == sio_helpers::SIOTocMarker) {
    const uint32_t position = firstWords & 0xffffffff;
    m_stream.seekg(position);

    sio::record_info recInfo;
    sio::buffer infoBuffer{sio::max_record_info_len};
    sio::buffer recBuffer{sio::mbyte};
    sio::api::read_record_info(m_stream, recInfo, infoBuffer);
    sio::api::read_record_data(m_stream, recInfo, recBuffer);

    sio::buffer uncBuffer{sio::mbyte};
    uncBuffer.resize(recInfo._uncompressed_length);
    sio::zlib_compression compressor;
    compressor.uncompress(recBuffer.span(), uncBuffer);

    sio::block_list blocks;
    auto tocBlock = std::make_shared<SIOFileTOCRecordBlock>();
    tocBlock->record = &m_tocRecord;
    blocks.push_back(tocBlock);

    sio::api::read_blocks(uncBuffer.span(), blocks);

    m_stream.seekg(0);
    return true;
  }

  m_stream.clear();
  m_stream.seekg(0);
  return false;
}

} // namespace podio
