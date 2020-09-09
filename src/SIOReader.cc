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
    // make sure that the first block is EventMetaData as it is also the first
    // during wrting
    m_blocks.push_back(m_eventMetaData);
  }

  CollectionBase* SIOReader::readCollection(const std::string& name) {
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


  void SIOReader::openFile(const std::string& filename){
    m_stream.open( filename , std::ios::binary ) ;
    readCollectionIDTable();
  }


  void SIOReader::closeFile(){
    m_stream.close() ;
  }

  void SIOReader::readEvent(){
    m_eventMetaData->metadata = new GenericParameters(); // will be managed by EventStore (?)

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
//   m_inputs.clear();
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
    const auto& typeNames = idTableBlock->getTypeNames();

    for (size_t i = 0; i < typeNames.size(); ++i) {
      auto blk = podio::SIOBlockFactory::instance().createBlock(typeNames[i], m_table->names()[i]);
      m_blocks.push_back(blk);
      m_inputs.emplace_back(std::make_pair(blk->getCollection(), m_table->names()[i]));
    }

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
} //namespace
