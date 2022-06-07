#include "podio/SIOBlock.h"

#include <algorithm>
#include <cstdlib>
#include <dlfcn.h>
#include <map>
#include <sstream>
#ifdef USE_BOOST_FILESYSTEM
  #include <boost/filesystem.hpp>
#else
  #include <filesystem>
#endif

namespace podio {
SIOCollectionIDTableBlock::SIOCollectionIDTableBlock(podio::EventStore* store) :
    sio::block("CollectionIDs", sio::version::encode_version(0, 3)) {
  const auto table = store->getCollectionIDTable();
  _names = table->names();
  _ids = table->ids();
  _types.reserve(_names.size());
  _isSubsetColl.reserve(_names.size());
  for (const int id : _ids) {
    CollectionBase* tmp;
    if (!store->get(id, tmp)) {
      std::cerr
          << "PODIO-ERROR cannot construct CollectionIDTableBlock because a collection is missing from the store (id: "
          << id << ", name: " << table->name(id) << ")" << std::endl;
    }

    _types.push_back(tmp->getValueTypeName());
    _isSubsetColl.push_back(tmp->isSubsetCollection());
  }
}

void SIOCollectionIDTableBlock::read(sio::read_device& device, sio::version_type version) {
  device.data(_names);
  device.data(_ids);
  device.data(_types);
  if (version >= sio::version::encode_version(0, 2)) {
    device.data(_isSubsetColl);
  }
}

void SIOCollectionIDTableBlock::write(sio::write_device& device) {
  device.data(_names);
  device.data(_ids);

  device.data(_types);
  device.data(_isSubsetColl);
}

template <typename MappedT>
void writeParamMap(sio::write_device& device, const GenericParameters::MapType<MappedT>& map) {
  device.data((int)map.size());
  for (const auto& [key, value] : map) {
    device.data(key);
    device.data(value);
  }
}

template <typename MappedT>
void readParamMap(sio::read_device& device, GenericParameters::MapType<MappedT>& map) {
  int size;
  device.data(size);
  while (size--) {
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

void SIOEventMetaDataBlock::read(sio::read_device& device, sio::version_type) {
  readGenericParameters(device, *metadata);
}

void SIOEventMetaDataBlock::write(sio::write_device& device) {
  writeGenericParameters(device, *metadata);
}

void SIONumberedMetaDataBlock::read(sio::read_device& device, sio::version_type) {
  int size;
  device.data(size);
  while (size--) {
    int id;
    device.data(id);
    GenericParameters params;
    readGenericParameters(device, params);

    data->emplace(id, std::move(params));
  }
}

void SIONumberedMetaDataBlock::write(sio::write_device& device) {
  device.data((int)data->size());
  for (const auto& [id, params] : *data) {
    device.data(id);
    writeGenericParameters(device, params);
  }
}

std::shared_ptr<SIOBlock> SIOBlockFactory::createBlock(const std::string& typeStr, const std::string& name,
                                                       const bool isSubsetColl) const {
  const auto it = _map.find(typeStr);

  if (it != _map.end()) {
    auto blk = std::shared_ptr<SIOBlock>(it->second->create(name));
    blk->createBuffers(isSubsetColl);
    return blk;
  } else {
    return nullptr;
  }
}

std::shared_ptr<SIOBlock> SIOBlockFactory::createBlock(const podio::CollectionBase* col,
                                                       const std::string& name) const {
  const std::string typeStr = col->getValueTypeName();
  const auto it = _map.find(typeStr);

  if (it != _map.end()) {
    auto blk = std::shared_ptr<SIOBlock>(it->second->create(name));
    blk->setCollection(const_cast<podio::CollectionBase*>(col));
    return blk;
  } else {
    return nullptr;
  }
}

SIOBlockLibraryLoader::SIOBlockLibraryLoader() {
  for (const auto& lib : getLibNames()) {
    loadLib(lib);
  }
}

void SIOBlockLibraryLoader::loadLib(const std::string& libname) {
  if (_loadedLibs.find(libname) != _loadedLibs.end()) {
    std::cerr << "SIOBlocks library \'" << libname << "\' already loaded. Not loading it again" << std::endl;
    return;
  }

  void* libhandle = dlopen(libname.c_str(), RTLD_LAZY | RTLD_GLOBAL);
  if (libhandle) {
    std::cout << "Loading SIOBlocks library \'" << libname << "\'" << std::endl;
    _loadedLibs.insert({libname, libhandle});
  } else {
    std::cerr << "ERROR while loading SIOBlocks library \'" << libname << "\'" << std::endl;
  }
}

std::vector<std::string> SIOBlockLibraryLoader::getLibNames() {
#ifdef USE_BOOST_FILESYSTEM
  namespace fs = boost::filesystem;
#else
  namespace fs = std::filesystem;
#endif
  std::vector<std::string> libs;

  std::string dir;
  const auto ldLibPath = std::getenv("LD_LIBRARY_PATH");
  if (!ldLibPath) {
    return libs;
  }
  std::istringstream stream(ldLibPath);
  while (std::getline(stream, dir, ':')) {
    if (not fs::exists(dir)) {
      continue;
    }

    for (auto& lib : fs::directory_iterator(dir)) {
      const auto filename = lib.path().filename().string();
      if (filename.find("SioBlocks") != std::string::npos) {
        libs.emplace_back(std::move(filename));
      }
    }
  }

  return libs;
}

void SIOFileTOCRecord::addRecord(const std::string& name, PositionType startPos) {
  auto it =
      std::find_if(m_recordMap.begin(), m_recordMap.end(), [&name](const auto& entry) { return entry.first == name; });

  if (it == m_recordMap.end()) {
    m_recordMap.push_back({name, {startPos}});
  } else {
    it->second.push_back(startPos);
  }
}

size_t SIOFileTOCRecord::getNRecords(const std::string& name) const {
  const auto it = std::find_if(m_recordMap.cbegin(), m_recordMap.cend(),
                               [&name](const auto& entry) { return entry.first == name; });
  if (it != m_recordMap.cend()) {
    return it->second.size();
  }
  return 0;
}

SIOFileTOCRecord::PositionType SIOFileTOCRecord::getPosition(const std::string& name, unsigned iEntry) const {
  const auto it = std::find_if(m_recordMap.cbegin(), m_recordMap.cend(),
                               [&name](const auto& keyVal) { return keyVal.first == name; });
  if (it != m_recordMap.end()) {
    return it->second[iEntry];
  }

  return 0;
}

void SIOFileTOCRecordBlock::read(sio::read_device& device, sio::version_type) {
  int size;
  device.data(size);
  while (size--) {
    std::string name;
    device.data(name);
    std::vector<SIOFileTOCRecord::PositionType> positions;
    device.data(positions);

    record->m_recordMap.emplace_back(std::move(name), std::move(positions));
  }
}

void SIOFileTOCRecordBlock::write(sio::write_device& device) {
  device.data((int)record->m_recordMap.size());
  for (const auto& [name, positions] : record->m_recordMap) {
    device.data(name);
    device.data(positions);
  }
}

} // namespace podio
