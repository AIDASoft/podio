#ifndef PODIO_SIOBLOCKUSERDATA_H
#define PODIO_SIOBLOCKUSERDATA_H

#include "podio/SIOBlock.h"
#include "podio/UserDataCollection.h"

#include <sio/api.h>
#include <sio/io_device.h>
#include <sio/version.h>

#include <algorithm>
#include <string>
#include <typeindex>

namespace {

/// helper function to get valid sio block names
inline std::string sio_name(const std::string& str) {
  std::string s = str;
  std::replace(s.begin(), s.end(), ' ', '_');
  return s;
}
} // namespace

namespace podio {

template <typename BasicType>
class SIOBlockUserData : public podio::SIOBlock {
public:
  SIOBlockUserData() : SIOBlock(::sio_name(podio::userDataTypeName<BasicType>()), sio::version::encode_version(0, 1)) {

    podio::SIOBlockFactory::instance().registerBlockForCollection(podio::userDataTypeName<BasicType>(), this);
  }

  SIOBlockUserData(const std::string& name) : SIOBlock(name, sio::version::encode_version(0, 1)) {
  }

  void read(sio::read_device& device, sio::version_type /*version*/) override {
    auto collBuffers = _col->getBuffers();
    auto* dataVec = collBuffers.dataAsVector<BasicType>();
    unsigned size(0);
    device.data(size);
    dataVec->resize(size);
    podio::handlePODDataSIO(device, &(*dataVec)[0], size);
  }

  void write(sio::write_device& device) override {
    _col->prepareForWrite();
    auto collBuffers = _col->getBuffers();
    auto* dataVec = collBuffers.dataAsVector<BasicType>();
    unsigned size = dataVec->size();
    device.data(size);
    podio::handlePODDataSIO(device, &(*dataVec)[0], size);
  }

  void createCollection(const bool) override {
    setCollection(new podio::UserDataCollection<BasicType>);
  }

  SIOBlock* create(const std::string& name) const override {
    return new SIOBlockUserData(name);
  }
};

} // namespace podio
#endif
