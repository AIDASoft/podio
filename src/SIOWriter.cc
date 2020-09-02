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
    m_store(store) {

    m_stream.open( filename , std::ios::binary ) ;

    if( not m_stream.is_open() ){
      SIO_THROW( sio::error_code::not_open, "Couldn't open output stream '" + filename + "'" ) ;
    }

    m_metaDataBlock = new SIOMetaDataBlock( "meta_data" , sio::version::encode_version(0, 1) ) ;
  }

  SIOWriter::~SIOWriter(){
  }

  void SIOWriter::writeMetaData(){

    auto* table =  m_store->getCollectionIDTable() ;

    std::cout << " writing meta data: \n" ;
    for(auto b :  m_blocks){
      std::string name = b->name() ;
      std::string typeName = static_cast<SIOBlock*>(b.get())->getCollection()->getValueTypeName() ;
      int id = table->collectionID( name ) ;
      m_metaDataBlock->add( id , name , typeName  ) ;

      std::cout << "\t " << id << ", " << name  << ", " << typeName << "\n" ;
    }
    // now write meta data record to file

    m_buffer.clear() ;
    sio::block_list blocks {} ;
    blocks.push_back( std::shared_ptr<sio::block>( m_metaDataBlock ) ) ;

    auto rec_info = sio::api::write_record( "metadata_record", m_buffer, blocks, 0 ) ;

    sio::api::write_record( m_stream, m_buffer.span(), rec_info ) ;

  }

  void SIOWriter::writeEvent(){

    if( ! m_metaData ){
      writeMetaData() ;
      m_metaData = true ;
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
    // now we want to safe the metadata
    // m_metadatatree->Branch("CollectionIDs",m_store->getCollectionIDTable());
    // m_metadatatree->Fill();
    // m_file->Write();

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

} // namespace
