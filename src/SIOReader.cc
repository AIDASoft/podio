// podio specific includes
#include "podio/SIOReader.h"

#include "podio/CollectionIDTable.h"
#include "podio/CollectionBase.h"

#include <sio/compression/zlib.h>

namespace podio {


  CollectionBase* SIOReader::readCollection(const std::string& name) {

    // if( m_lastEvtRead != m_eventNumber ){
    //   readEvent() ;
    //   m_lastEvtRead = m_eventNumber ;
    // }

    auto p = std::find_if(begin(m_inputs), end(m_inputs),
                          [&name](SIOReader::Input t){ return t.second == name;});

    return ( p != end(m_inputs) ?  p->first : nullptr ) ;
  }


  void SIOReader::openFile(const std::string& filename){
    m_stream.open( filename , std::ios::binary ) ;
    readCollectionIDTable();
  }


  void SIOReader::closeFile(){
    m_stream.close() ;
  }


  GenericParameters* SIOReader::readEventMetaData(){
    // TODO
    return new GenericParameters();
  }
  std::map<int,GenericParameters>* SIOReader::readCollectionMetaData(){
    // TODO
    return new std::map<int, GenericParameters>();
  }
  std::map<int,GenericParameters>* SIOReader::readRunMetaData(){
    // TODO
    return new std::map<int, GenericParameters>();
  }


  void SIOReader::readEvent(){

    sio::record_info rec_info ;
    sio::api::read_record_info( m_stream, rec_info, m_info_buffer ) ;
    sio::api::read_record_data( m_stream, rec_info, m_rec_buffer ) ;

    m_unc_buffer.resize( rec_info._uncompressed_length ) ;
    sio::zlib_compression compressor ;
    compressor.uncompress( m_rec_buffer.span(), m_unc_buffer ) ;

    sio::api::read_blocks( m_unc_buffer.span(), m_blocks ) ;

    for( auto bl : m_blocks ){ // creates the object layer
      static_cast<SIOBlock*>(bl.get())->prepareAfterRead();
    }
    for( auto bl : m_blocks ){  // resolves the references
      static_cast<SIOBlock*>(bl.get())->setReferences();
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

    m_table = static_cast<SIOCollectionIDTableBlock*>(blocks[0].get())->get();
  }

} //namespace
