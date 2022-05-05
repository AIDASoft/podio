#ifndef PODIO_SIOBLOCKUSERDATA_H
#define PODIO_SIOBLOCKUSERDATA_H

#include "podio/CollectionBuffers.h"
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
template <typename BasicType, typename = podio::EnableIfSupportedUserType<BasicType>>
inline std::string sio_name() {
  std::string s = podio::userDataTypeName<BasicType>();
  std::replace(s.begin(), s.end(), ' ', '_');
  return s;
}
} // namespace

namespace podio {

template <typename BasicType, typename = EnableIfSupportedUserType<BasicType>>
class SIOBlockUserData : public podio::SIOBlock {
public:
  SIOBlockUserData() : SIOBlock(::sio_name<BasicType>(), sio::version::encode_version(0, 1)) {

    podio::SIOBlockFactory::instance().registerBlockForCollection(podio::userDataTypeName<BasicType>(), this);
  }

  SIOBlockUserData(const std::string& name) : SIOBlock(name, sio::version::encode_version(0, 1)) {
  }

  void read(sio::read_device& device, sio::version_type /*version*/) override {
    auto* dataVec = new std::vector<BasicType>();
    unsigned size(0);
    device.data(size);
    dataVec->resize(size);
    podio::handlePODDataSIO(device, &(*dataVec)[0], size);
    m_buffers.data = dataVec;
  }

  void write(sio::write_device& device) override {
    auto* dataVec = podio::CollectionWriteBuffers::asVector<BasicType>(m_buffers.data);
    unsigned size = dataVec->size();
    device.data(size);
    podio::handlePODDataSIO(device, &(*dataVec)[0], size);
  }

  void createBuffers(bool) override {

    m_buffers.references = new podio::CollRefCollection();
    m_buffers.vectorMembers = new podio::VectorMembersInfo();

    // Nothing to do here since UserDataCollections cannot be subset collections
    m_buffers.createCollection = [](podio::CollectionReadBuffers buffers, bool) {
      return std::make_unique<UserDataCollection<BasicType>>(std::move(*buffers.dataAsVector<BasicType>()));
    };
  }

  SIOBlock* create(const std::string& name) const override {
    return new SIOBlockUserData(name);
  }

private:
};

} // namespace podio
#endif
