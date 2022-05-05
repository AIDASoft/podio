// podio specific includes
#include "podio/SIOWriter.h"
#include "podio/CollectionBase.h"
#include "podio/EventStore.h"
#include "podio/SIOBlock.h"

// SIO specifc includes
#include "sio/block.h"
#include "sio/compression/zlib.h"

namespace podio {

SIOWriter::SIOWriter(const std::string& filename, EventStore* store) :
    m_filename(filename),
    m_store(store),
    m_eventMetaData(std::make_shared<SIOEventMetaDataBlock>()),
    m_runMetaData(std::make_shared<SIONumberedMetaDataBlock>("RunMetaData")),
    m_collectionMetaData(std::make_shared<SIONumberedMetaDataBlock>("CollectionMetaData")) {

  m_stream.open(filename, std::ios::binary);

  if (not m_stream.is_open()) {
    SIO_THROW(sio::error_code::not_open, "Couldn't open output stream '" + filename + "'");
  }

  // TODO: re-visit this once metadata handling is done in better defined way
  m_eventMetaData->metadata = m_store->eventMetaDataPtr();

  m_runMetaData->data = m_store->getRunMetaDataMap();
  m_collectionMetaData->data = m_store->getColMetaDataMap();

  auto& libLoader [[maybe_unused]] = SIOBlockLibraryLoader::instance();
}

void SIOWriter::writeEvent() {
  if (m_firstEvent) {
    // Write the collectionIDs as a separate record at the beginning of the
    // file. In this way they can easily be retrieved in the SIOReader without
    // having to look for this specific record.
    writeCollectionIDTable();
    m_firstEvent = false;
  }

  m_buffer.clear();
  m_com_buffer.clear();

  auto blocks = createBlocks();

  // write the record to the sio buffer
  auto rec_info = sio::api::write_record("event_record", m_buffer, blocks, 0);

  // use zlib to compress the record into another buffer
  sio::zlib_compression compressor;
  compressor.set_level(6); // Z_DEFAULT_COMPRESSION==6
  sio::api::compress_record(rec_info, m_buffer, m_com_buffer, compressor);

  // and now write record to the file !
  sio::api::write_record(m_stream, m_buffer.span(0, rec_info._header_length), m_com_buffer.span(), rec_info);

  m_tocRecord.addRecord("event_record", rec_info._file_start);
}

sio::block_list SIOWriter::createBlocks() {
  sio::block_list blocks;
  blocks.emplace_back(m_eventMetaData);

  for (const auto& name : m_collectionsToWrite) {
    const podio::CollectionBase* col{nullptr};
    m_store->get(name, col);
    const_cast<podio::CollectionBase*>(col)->prepareForWrite();

    blocks.emplace_back(podio::SIOBlockFactory::instance().createBlock(col, name));
  }

  return blocks;
}

void SIOWriter::finish() {
  m_buffer.clear();
  m_com_buffer.clear();

  sio::block_list blocks{};
  blocks.push_back(m_runMetaData);

  auto rec_info = sio::api::write_record(m_runMetaData->name(), m_buffer, blocks, 0);
  sio::zlib_compression compressor;
  compressor.set_level(6);
  sio::api::compress_record(rec_info, m_buffer, m_com_buffer, compressor);
  sio::api::write_record(m_stream, m_buffer.span(0, rec_info._header_length), m_com_buffer.span(), rec_info);

  m_tocRecord.addRecord(m_runMetaData->name(), rec_info._file_start);

  blocks.clear();
  m_buffer.clear();
  m_com_buffer.clear();

  blocks.push_back(m_collectionMetaData);
  rec_info = sio::api::write_record(m_collectionMetaData->name(), m_buffer, blocks, 0);
  sio::api::compress_record(rec_info, m_buffer, m_com_buffer, compressor);
  sio::api::write_record(m_stream, m_buffer.span(0, rec_info._header_length), m_com_buffer.span(), rec_info);

  m_tocRecord.addRecord(m_collectionMetaData->name(), rec_info._file_start);

  blocks.clear();
  m_buffer.clear();
  m_com_buffer.clear();

  auto tocRecordBlock = std::make_shared<SIOFileTOCRecordBlock>();
  tocRecordBlock->record = &m_tocRecord;
  blocks.push_back(tocRecordBlock);

  rec_info = sio::api::write_record(sio_helpers::SIOTocRecordName, m_buffer, blocks, 0);
  sio::api::compress_record(rec_info, m_buffer, m_com_buffer, compressor);
  sio::api::write_record(m_stream, m_buffer.span(0, rec_info._header_length), m_com_buffer.span(), rec_info);

  // Now that we know the position of the TOC Record, put this information
  // into a final marker that can be identified and interpreted when reading
  // again
  uint64_t finalWords = (((uint64_t)sio_helpers::SIOTocMarker) << 32) | ((uint64_t)rec_info._file_start & 0xffffffff);
  m_stream.write(reinterpret_cast<char*>(&finalWords), sizeof(finalWords));

  m_stream.close();
}

void SIOWriter::registerForWrite(const std::string& name) {

  const podio::CollectionBase* colB(nullptr);
  m_store->get(name, colB);

  if (!colB) {
    throw std::runtime_error(std::string("no such collection to write: ") + name);
  }
  // Check if we can instantiate the blocks here so that we can skip the checks later
  if (auto blk = podio::SIOBlockFactory::instance().createBlock(colB, name); !blk) {
    const auto typName = colB->getValueTypeName();
    throw std::runtime_error(std::string("could not create SIOBlock for type: ") + typName);
  }

  m_collectionsToWrite.push_back(name);
}

void SIOWriter::writeCollectionIDTable() {
  m_buffer.clear();
  m_com_buffer.clear();
  sio::block_list blocks;
  blocks.emplace_back(std::make_shared<SIOCollectionIDTableBlock>(m_store));
  blocks.emplace_back(std::make_shared<SIOVersionBlock>(podio::version::build_version));

  auto rec_info = sio::api::write_record("file_metadata", m_buffer, blocks, 0);

  sio::zlib_compression compressor;
  compressor.set_level(6);
  sio::api::compress_record(rec_info, m_buffer, m_com_buffer, compressor);

  sio::api::write_record(m_stream, m_buffer.span(0, rec_info._header_length), m_com_buffer.span(), rec_info);

  m_tocRecord.addRecord("file_metadata", rec_info._file_start);
}

} // namespace podio
