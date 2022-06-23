#include "podio/SIOFrameReader.h"
#include "podio/SIOBlock.h"

#include <sio/api.h>
#include <sio/compression/zlib.h>
#include <sio/definitions.h>

#include <utility>

namespace podio {

namespace sio_utils {
  // Read the record into a buffer and potentially uncompress it
  std::pair<sio::buffer, sio::record_info> readRecord(sio::ifstream& stream, bool decompress = true,
                                                      std::size_t initBufferSize = sio::mbyte) {
    sio::record_info recInfo;
    sio::buffer infoBuffer{sio::max_record_info_len};
    sio::buffer recBuffer{initBufferSize};
    sio::api::read_record_info(stream, recInfo, infoBuffer);
    sio::api::read_record_data(stream, recInfo, recBuffer);

    if (decompress) {
      sio::buffer uncBuffer{recInfo._uncompressed_length};
      sio::zlib_compression compressor;
      compressor.uncompress(recBuffer.span(), uncBuffer);
      return std::make_pair(std::move(uncBuffer), recInfo);
    }

    return std::make_pair(std::move(recBuffer), recInfo);
  }
} // namespace sio_utils

SIOFrameReader::SIOFrameReader() {
  auto& libLoader [[maybe_unused]] = SIOBlockLibraryLoader::instance();
}

void SIOFrameReader::openFile(const std::string& filename) {
  m_stream.open(filename, std::ios::binary);
  if (!m_stream.is_open()) {
    SIO_THROW(sio::error_code::not_open, "Cannot open input file '" + filename + "' for reading");
  }

  // NOTE: reading TOC record first because that jumps back to the start of the file!
  readFileTOCRecord();
  readPodioHeader();
}

std::unique_ptr<SIORawData> SIOFrameReader::readNextEntry(const std::string& name) {
  // Skip to where the next record of this category starts in the file, based on
  // how many times we have already read this category
  //
  // NOTE: exploiting the fact that the operator[] of a map will create a
  // default initialized entry for us if not present yet
  const auto recordPos = m_tocRecord.getPosition(name, m_categoryCtr[name]);
  if (recordPos == 0) {
    return nullptr;
  }
  m_stream.seekg(recordPos);

  auto [tableBuffer, tableInfo] = sio_utils::readRecord(m_stream, false);
  auto [dataBuffer, dataInfo] = sio_utils::readRecord(m_stream, false);

  m_categoryCtr[name]++;

  return std::make_unique<SIORawData>(std::move(dataBuffer), dataInfo._uncompressed_length, std::move(tableBuffer),
                                      tableInfo._uncompressed_length);
}

unsigned SIOFrameReader::getEntries(const std::string& category) const {
  return m_tocRecord.getNRecords(category);
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

    const auto& [uncBuffer, _] = sio_utils::readRecord(m_stream);

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

void SIOFrameReader::readPodioHeader() {
  const auto& [buffer, _] = sio_utils::readRecord(m_stream, false, sizeof(podio::version::Version));

  sio::block_list blocks;
  blocks.emplace_back(std::make_shared<SIOVersionBlock>());
  sio::api::read_blocks(buffer.span(), blocks);

  m_fileVersion = static_cast<SIOVersionBlock*>(blocks[0].get())->version;
}

} // namespace podio
