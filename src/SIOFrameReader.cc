#include "podio/SIOFrameReader.h"
#include "podio/SIOBlock.h"

#include "sioUtils.h"

#include <sio/api.h>
#include <sio/definitions.h>

#include <utility>

namespace podio {

SIOFrameReader::SIOFrameReader() {
  auto& libLoader [[maybe_unused]] = SIOBlockLibraryLoader::instance();
}

void SIOFrameReader::openFile(const std::string& filename) {
  m_stream.open(filename, std::ios::binary);
  if (!m_stream.is_open()) {
    throw std::runtime_error("File " + filename + " couldn't be opened");
  }

  // NOTE: reading TOC record first because that jumps back to the start of the file!
  readFileTOCRecord();
  readPodioHeader();
}

std::unique_ptr<SIOFrameData> SIOFrameReader::readNextEntry(const std::string& name) {
  // Skip to where the next record of this name starts in the file, based on
  // how many times we have already read this name
  //
  // NOTE: exploiting the fact that the operator[] of a map will create a
  // default initialized entry for us if not present yet
  const auto recordPos = m_tocRecord.getPosition(name, m_nameCtr[name]);
  if (recordPos == 0) {
    return nullptr;
  }
  m_stream.seekg(recordPos);

  auto [tableBuffer, tableInfo] = sio_utils::readRecord(m_stream, false);
  auto [dataBuffer, dataInfo] = sio_utils::readRecord(m_stream, false);

  m_nameCtr[name]++;

  return std::make_unique<SIOFrameData>(std::move(dataBuffer), dataInfo._uncompressed_length, std::move(tableBuffer),
                                        tableInfo._uncompressed_length);
}

std::unique_ptr<SIOFrameData> SIOFrameReader::readEntry(const std::string& name, const unsigned entry) {
  // NOTE: Will create or overwrite the entry counter
  //       All checks are done in the following function
  m_nameCtr[name] = entry;
  return readNextEntry(name);
}

std::vector<std::string_view> SIOFrameReader::getAvailableCategories() const {
  return m_tocRecord.getRecordNames();
}

unsigned SIOFrameReader::getEntries(const std::string& name) const {
  return m_tocRecord.getNRecords(name);
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
