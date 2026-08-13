#include "podio/SIOBlock.h"

#include "podio/utilities/BackendLibraryLoader.h"
#include <algorithm>

namespace podio {

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

void writeGenericParameters(sio::write_device& device, const GenericParameters& params) {
  writeMapLike(device, params.getMap<int>());
  writeMapLike(device, params.getMap<float>());
  writeMapLike(device, params.getMap<std::string>());
  writeMapLike(device, params.getMap<double>());
}

void readGenericParameters(sio::read_device& device, GenericParameters& params, sio::version_type version) {
  readMapLike(device, params.getMap<int>());
  readMapLike(device, params.getMap<float>());
  readMapLike(device, params.getMap<std::string>());
  if (version >= sio::version::encode_version(0, 2)) {
    readMapLike(device, params.getMap<double>());
  }
}

void SIOEventMetaDataBlock::read(sio::read_device& device, sio::version_type version) {
  readGenericParameters(device, *metadata, version);
}

void SIOEventMetaDataBlock::write(sio::write_device& device) {
  writeGenericParameters(device, *metadata);
}

void SIONumberedMetaDataBlock::read(sio::read_device& device, sio::version_type version) {
  int size;
  device.data(size);
  while (size--) {
    int id;
    device.data(id);
    GenericParameters params;
    readGenericParameters(device, params, version);

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
    blk->setSubsetCollection(isSubsetColl);
    return blk;
  } else {
    return nullptr;
  }
}

std::shared_ptr<SIOBlock> SIOBlockFactory::createBlock(const podio::CollectionBase* col,
                                                       const std::string& name) const {
  const auto typeStr = std::string(col->getValueTypeName()); // Need c++20 for transparent lookup
  const auto it = _map.find(typeStr);

  if (it != _map.end()) {
    auto blk = std::shared_ptr<SIOBlock>(it->second->create(name));
    blk->setCollection(const_cast<podio::CollectionBase*>(col));
    return blk;
  } else {
    return nullptr;
  }
}
void loadSIOBlocksLibraries() {
  static podio::utilities::BackendLibraryLoader me("PODIO_SIOBLOCK_PATH", "SioBlocks", "SIOBlocks");
}

void SIOFileTOCRecord::addRecord(const std::string& name, PositionType startPos) {
  auto it = std::ranges::find(m_recordMap, name, &decltype(m_recordMap)::value_type::first);

  if (it == m_recordMap.end()) {
    m_recordMap.push_back({name, {startPos}});
  } else {
    it->second.push_back(startPos);
  }
}

size_t SIOFileTOCRecord::getNRecords(std::string_view name) const {
  const auto it = std::ranges::find(m_recordMap, name, &decltype(m_recordMap)::value_type::first);
  if (it != m_recordMap.cend()) {
    return it->second.size();
  }
  return 0;
}

SIOFileTOCRecord::PositionType SIOFileTOCRecord::getPosition(std::string_view name, unsigned iEntry) const {
  const auto it = std::ranges::find(m_recordMap, name, &decltype(m_recordMap)::value_type::first);
  if (it != m_recordMap.end()) {
    if (iEntry < it->second.size()) {
      return it->second[iEntry];
    }
  }

  return 0;
}

std::vector<std::string_view> SIOFileTOCRecord::getRecordNames() const {
  std::vector<std::string_view> cats;
  cats.reserve(m_recordMap.size());
  for (const auto& [cat, _] : m_recordMap) {
    cats.emplace_back(cat);
  }

  return cats;
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
