#include "podio/SIOBlock.h"

namespace podio {
  void SIOCollectionIDTableBlock::read(sio::read_device& device, sio::version_type version) {
    std::vector<std::string> names;
    std::vector<int> ids;
    device.data(names);
    device.data(ids);

    _table = new CollectionIDTable(ids, names);
  }

  void SIOCollectionIDTableBlock::write(sio::write_device& device) {
    device.data(_table->names());
    device.data(_table->ids());
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
