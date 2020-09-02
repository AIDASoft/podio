#include "podio/SIOBlock.h"

namespace podio {

  void SIOMetaDataBlock::read(sio::read_device& device, sio::version_type vers) {
    unsigned size(0);
    device.data( size );
    m_vec.resize(size);
    for(unsigned i=0; i<size;++i){
      device.data( m_vec[i].colID );
      device.data( m_vec[i].name );
      device.data( m_vec[i].typeName );
    }
  }

  void SIOMetaDataBlock::write(sio::write_device& device) {
    unsigned size = m_vec.size() ;
    device.data( size );
    for(unsigned i = 0; i < size; ++i){
      device.data( m_vec[i].colID );
      device.data( m_vec[i].name );
      device.data( m_vec[i].typeName );
    }
  }


  std::shared_ptr<SIOBlock> SIOBlockFactory::createBlock(const std::string& typeStr, const std::string& name) const {
    const auto it = _map.find(typeStr) ;

    if( it != _map.end() ){
      auto blk = std::shared_ptr<SIOBlock>(it->second->create( name ));
      blk->createCollection() ;
      return blk;
    } else {
      return nullptr;
    }
  }

  std::shared_ptr<SIOBlock> SIOBlockFactory::createBlock(const podio::CollectionBase* col, const std::string& name) const {
    const std::string typeStr = col->getValueTypeName() ;
    const auto it = _map.find(typeStr) ;

    if( it!= _map.end() ) {
      auto blk = std::shared_ptr<SIOBlock>(it->second->create(name));
      blk->setCollection( const_cast< podio::CollectionBase* > ( col ) ) ;
      return blk;
    } else {
      return nullptr;
    }
  }

}
