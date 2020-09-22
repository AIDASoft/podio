// podio specific includes
#include "podio/SIOReader.h"

#include "podio/CollectionIDTable.h"
#include "podio/CollectionBase.h"
#include "podio/EventStore.h"
#include "podio/SIOBlock.h"
#include "sio/definitions.h"

#include <memory>
#include <sio/compression/zlib.h>

namespace podio {

  SIOReader::SIOReader() :
    m_eventNumber(0),
    m_eventMetaData(std::make_shared<SIOEventMetaDataBlock>()),
    m_runMetaData(std::make_shared<SIONumberedMetaDataBlock>("RunMetaData")),
    m_collectionMetaData(std::make_shared<SIONumberedMetaDataBlock>("CollectionMetaData"))
  {
    auto& libLoader = SIOBlockLibraryLoader::instance();
  }

  CollectionBase* SIOReader::readCollection(const std::string& name) {
    if (m_lastEventRead != m_eventNumber) {
      readEvent();
    }

    auto p = std::find_if(begin(m_inputs), end(m_inputs),
                          [&name](const SIOReader::Input& t){ return t.second == name;});

    if (p != end(m_inputs)) {
      p->first->prepareAfterRead();
      return p->first;
    }

    return nullptr;
  }

  std::map<int,GenericParameters>* SIOReader::readCollectionMetaData() {
    // Only read the data if it hasn't been read already
    if (!m_collectionMetaData->data) {
      m_collectionMetaData->data = new ColMDMap();
      readMetaDataRecord(m_collectionMetaData);
    }

    return m_collectionMetaData->data;
  }

  std::map<int,GenericParameters>* SIOReader::readRunMetaData() {
    // Only read the data if it hasn't been read already
    if (!m_runMetaData->data) {
      m_runMetaData->data = new RunMDMap();
      readMetaDataRecord(m_runMetaData);
    }

    return m_runMetaData->data;
  }

  podio::GenericParameters* SIOReader::readEventMetaData() {
    if (m_lastEventRead != m_eventNumber) {
      readEvent();
    }
    return m_eventMetaData->metadata;
  }

  void SIOReader::openFile(const std::string& filename){
    m_stream.open( filename , std::ios::binary ) ;
    readCollectionIDTable();

    if (!readFileTOCRecord()) {
      reconstructFileTOCRecord();
    }
  }


  void SIOReader::closeFile(){
    m_stream.close() ;
  }

  void SIOReader::readEvent(){
    // recreate the blocks, since the contents are owned and managed by the
    // EventStore
    createBlocks();

    // skip possible intermediate records that are not event data
    sio::api::go_to_record(m_stream, "event_record");

    sio::record_info rec_info ;
    sio::api::read_record_info( m_stream, rec_info, m_info_buffer ) ;
    sio::api::read_record_data( m_stream, rec_info, m_rec_buffer ) ;

    m_unc_buffer.resize( rec_info._uncompressed_length ) ;
    sio::zlib_compression compressor ;
    compressor.uncompress( m_rec_buffer.span(), m_unc_buffer ) ;
    sio::api::read_blocks( m_unc_buffer.span(), m_blocks ) ;

    for (auto& [collection, name] : m_inputs) {
      collection->setID(m_table->collectionID(name));
    }

    m_lastEventRead = m_eventNumber;
  }

  bool SIOReader::isValid() const {
    return m_stream.good()  ;
  }

  SIOReader::~SIOReader() {
    // delete all collections
    // at the moment it is done in the EventStore;
    // TODO: who deletes the buffers?
  }

  void SIOReader::endOfEvent() {
    ++m_eventNumber;
    m_blocks.clear();
    m_inputs.clear();
  }

  void SIOReader::createBlocks() {
    // make sure that the first block is EventMetaData as it is also the first
    // during wrting
    m_eventMetaData->metadata = new GenericParameters(); // will be managed by EventStore (?)
    m_blocks.push_back(m_eventMetaData);

    for (size_t i = 0; i < m_typeNames.size(); ++i) {
      auto blk = podio::SIOBlockFactory::instance().createBlock(m_typeNames[i], m_table->names()[i]);
      m_blocks.push_back(blk);
      m_inputs.emplace_back(blk->getCollection(), m_table->names()[i]);
    }
  }

  void SIOReader::readCollectionIDTable() {
    sio::record_info rec_info;
    sio::api::read_record_info(m_stream, rec_info, m_info_buffer);
    sio::api::read_record_data(m_stream, rec_info, m_rec_buffer);

    m_unc_buffer.resize(rec_info._uncompressed_length);
    sio::zlib_compression compressor;
    compressor.uncompress(m_rec_buffer.span(), m_unc_buffer);

    sio::block_list blocks;
    blocks.emplace_back(std::make_shared<SIOCollectionIDTableBlock>());
    sio::api::read_blocks(m_unc_buffer.span(), blocks);

    auto* idTableBlock = static_cast<SIOCollectionIDTableBlock*>(blocks[0].get());
    m_table = idTableBlock->getTable();
    m_typeNames = idTableBlock->getTypeNames();
  }

  void SIOReader::readMetaDataRecord(std::shared_ptr<SIONumberedMetaDataBlock> mdBlock) {
    const auto currPos = m_stream.tellg();
    sio::api::go_to_record(m_stream, mdBlock->name());

    sio::record_info rec_info;
    sio::api::read_record_info(m_stream, rec_info, m_info_buffer);
    sio::api::read_record_data(m_stream, rec_info, m_rec_buffer);

    m_unc_buffer.resize(rec_info._uncompressed_length);
    sio::zlib_compression compressor;
    compressor.uncompress(m_rec_buffer.span(), m_unc_buffer);

    sio::block_list blocks{};
    blocks.push_back(mdBlock);
    sio::api::read_blocks(m_unc_buffer.span(), blocks);

    m_stream.seekg(currPos);
  }

  void SIOReader::reconstructFileTOCRecord() {
    try {
      // use a simple unary predicate that always returns true, and hence skips
      // over all records, but as a sideffect populates the tocRecord
      sio::api::skip_records(m_stream,
                             [&](const sio::record_info& rec_info) {
                               m_tocRecord.addRecord(rec_info._name, rec_info._file_start);
                               return true;
                             });
    } catch (sio::exception& e) {
      if (e.code() != sio::error_code::eof) {
        SIO_RETHROW(e, e.code(), e.what());
      }
    }

    // rewind to the start of the file
    m_stream.clear();
    m_stream.seekg(0);
  }

  bool SIOReader::readFileTOCRecord() {
    // Check if there is a dedicated marker at the end of the file that tells us
    // where the TOC actually starts
    m_stream.seekg(-sio_helpers::SIOTocInfoSize, std::ios_base::end);
    uint64_t firstWords{0};
    m_stream.read(reinterpret_cast<char*>(&firstWords), sizeof(firstWords));

    const uint32_t marker = (firstWords >> 32) & 0xffffffff;
    if (marker == sio_helpers::SIOTocMarker) {
      const uint32_t position = firstWords & 0xffffffff;
      m_stream.seekg(position);

      sio::record_info rec_info;
      sio::api::read_record_info(m_stream, rec_info, m_info_buffer);
      sio::api::read_record_data(m_stream, rec_info, m_rec_buffer);

      m_unc_buffer.resize(rec_info._uncompressed_length);
      sio::zlib_compression compressor;
      compressor.uncompress(m_rec_buffer.span(), m_unc_buffer);

      sio::block_list blocks;
      auto tocBlock = std::make_shared<SIOFileTOCRecordBlock>();
      tocBlock->record = &m_tocRecord;
      blocks.push_back(tocBlock);

      sio::api::read_blocks(m_unc_buffer.span(), blocks);

      m_unc_buffer.clear();
      m_rec_buffer.clear();
      m_stream.seekg(0);
      return true;
    }

    m_stream.clear();
    m_stream.seekg(0);
    return false;
  }
} //namespace
