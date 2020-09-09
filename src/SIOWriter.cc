// podio specific includes
#include "podio/CollectionBase.h"
#include "podio/EventStore.h"
#include "podio/SIOWriter.h"
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
    m_collectionMetaData(std::make_shared<SIONumberedMetaDataBlock>("CollectionMetaData"))
  {

    m_stream.open( filename , std::ios::binary ) ;

    if( not m_stream.is_open() ){
      SIO_THROW( sio::error_code::not_open, "Couldn't open output stream '" + filename + "'" ) ;
    }

    m_eventMetaData->metadata = m_store->eventMetaDataPtr();
    m_blocks.push_back(m_eventMetaData);

    m_runMetaData->data = m_store->getRunMetaDataMap();
    m_collectionMetaData->data = m_store->getColMetaDataMap();
  }

  SIOWriter::~SIOWriter(){
  }


  void SIOWriter::writeEvent(){
    if (m_firstEvent) {
      // Write the collectionIDs as a separate record at the beginning of the
      // file. In this way they can easily be retrieved in the SIOReader without
      // having to look for this specific record.
      writeCollectionIDTable();
      m_firstEvent = false;
    }

    m_buffer.clear() ;
    m_com_buffer.clear() ;

    // for (auto& coll : m_storedCollections){
    //   coll->prepareForWrite();
    // }
    // write the record to the sio buffer
    auto rec_info = sio::api::write_record( "event_record", m_buffer, m_blocks, 0 ) ;

    // use zlib to compress the record into another buffer
    sio::zlib_compression compressor ;
    compressor.set_level( 6 ) ;  // Z_DEFAULT_COMPRESSION==6
    sio::api::compress_record( rec_info, m_buffer, m_com_buffer, compressor ) ;

    // and now write record to the file !
    sio::api::write_record( m_stream, m_buffer.span(0, rec_info._header_length), m_com_buffer.span(), rec_info ) ;
  }

  void SIOWriter::finish(){
    m_buffer.clear();
    m_com_buffer.clear();

    sio::block_list blocks{};
    blocks.push_back(m_runMetaData);

    auto rec_info = sio::api::write_record(m_runMetaData->name(), m_buffer, blocks, 0);
    sio::zlib_compression compressor;
    compressor.set_level(6);
    sio::api::compress_record(rec_info, m_buffer, m_com_buffer, compressor);
    sio::api::write_record(m_stream, m_buffer.span(0, rec_info._header_length), m_com_buffer.span(), rec_info);

    blocks.clear();
    m_buffer.clear();
    m_com_buffer.clear();

    blocks.push_back(m_collectionMetaData);
    rec_info = sio::api::write_record(m_collectionMetaData->name(), m_buffer, blocks, 0);
    sio::api::compress_record(rec_info, m_buffer, m_com_buffer, compressor);
    sio::api::write_record(m_stream, m_buffer.span(0, rec_info._header_length), m_com_buffer.span(), rec_info);

    m_stream.close() ;
  }

  void SIOWriter::registerForWrite(const std::string& name){

    const podio::CollectionBase* colB(nullptr) ;
    m_store->get( name , colB );

    if( !colB ){
      throw std::runtime_error( std::string("no such collection to write: ")+name ) ;
    }
    auto blk = podio::SIOBlockFactory::instance().createBlock( colB, name ) ;

    if( !blk ){
      std::string typName = podio::demangleClassName( colB ) ;
      throw std::runtime_error( std::string("could not create SIOBlock for type: ")+typName ) ;
    }
    m_blocks.push_back(blk) ;
  }

  void SIOWriter::writeCollectionIDTable() {
    m_buffer.clear();
    m_com_buffer.clear();
    sio::block_list blocks;
    blocks.emplace_back(std::make_shared<SIOCollectionIDTableBlock>(m_store));

    auto rec_info = sio::api::write_record("CollectionIDs", m_buffer, blocks, 0);

    sio::zlib_compression compressor;
    compressor.set_level(6);
    sio::api::compress_record(rec_info, m_buffer, m_com_buffer, compressor);

    sio::api::write_record(m_stream, m_buffer.span(0, rec_info._header_length), m_com_buffer.span(), rec_info);

  }

} // namespace
