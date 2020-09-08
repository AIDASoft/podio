#include "podio/SIOBlock.h"

#include <map>

namespace podio {
  void SIOCollectionIDTableBlock::read(sio::read_device& device, sio::version_type version) {
    std::vector<std::string> names;
    std::vector<int> ids;
    device.data(names);
    device.data(ids);
    device.data(_types);

    _table = new CollectionIDTable(ids, names);
  }

  void SIOCollectionIDTableBlock::write(sio::write_device& device) {
    device.data(_table->names());
    device.data(_table->ids());

    std::vector<std::string> typeNames;
    typeNames.reserve(_table->ids().size());
    for (const int id : _table->ids()) {
      CollectionBase* tmp;
      if (!_store->get(id, tmp)) {
        std::cerr << "ERROR during writing of CollectionID table" << std::endl;
      }
      typeNames.push_back(tmp->getValueTypeName());
    }
    device.data(typeNames);
  }

  template<typename MappedT>
  void writeParamMap(sio::write_device& device, const GenericParameters::MapType<MappedT>& map) {
    device.data((int) map.size());
    for (const auto& [key, value] : map) {
      device.data(key);
      device.data(value);
    }
  }

  template<typename MappedT>
  void readParamMap(sio::read_device& device, GenericParameters::MapType<MappedT>& map) {
    int size;
    device.data(size);
    while(size--) {
      std::string key;
      device.data(key);
      std::vector<MappedT> values;
      device.data(values);
      map.emplace(std::move(key), std::move(values));
    }
  }


  void writeGenericParameters(sio::write_device& device, const GenericParameters& params) {
    writeParamMap(device, params.getIntMap());
    writeParamMap(device, params.getFloatMap());
    writeParamMap(device, params.getStringMap());
  }

  void readGenericParameters(sio::read_device& device, GenericParameters& params) {
    readParamMap(device, params.getIntMap());
    readParamMap(device, params.getFloatMap());
    readParamMap(device, params.getStringMap());
  }


  void SIOEventMetaDataBlock::read(sio::read_device& device, sio::version_type version) {
    readGenericParameters(device, *metadata);
  }

  void SIOEventMetaDataBlock::write(sio::write_device& device) {
    writeGenericParameters(device, *metadata);
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
