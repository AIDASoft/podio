#ifndef PODIO_ASSOCIATIONSIOBLOCK_H
#define PODIO_ASSOCIATIONSIOBLOCK_H

#include "podio/AssociationFwd.h"
#include "podio/SIOBlock.h"

#include <sio/api.h>
#include <sio/io_device.h>
#include <sio/version.h>

#include <algorithm>
#include <string>

namespace podio {
template <typename FromT, typename ToT>
class AssociationSIOBlock : public podio::SIOBlock {
public:
  AssociationSIOBlock() :
      SIOBlock(podio::detail::associationSIOName<FromT, ToT>(), sio::version::encode_version(0, 1)) {
    podio::SIOBlockFactory::instance().registerBlockForCollection(podio::detail::associationSIOName<FromT, ToT>(),
                                                                  this);
  }

  AssociationSIOBlock(const std::string& name) : SIOBlock(name, sio::version::encode_version(0, 1)) {
  }

  void read(sio::read_device& device, sio::version_type) override {
    auto buffers = _col->getBuffers();
    if (!_col->isSubsetCollection()) {
      auto* dataVec = buffers.dataAsVector<float>();
      unsigned size{0};
      device.data(size);
      dataVec->resize(size);
      podio::handlePODDataSIO(device, dataVec->data(), size);
    }

    // ---- references
    auto* refColls = buffers.references;
    for (auto& refC : *refColls) {
      unsigned size{0};
      device.data(size);
      refC->resize(size);
      podio::handlePODDataSIO(device, refC->data(), size);
    }
  }

  void write(sio::write_device& device) override {
    _col->prepareForWrite();
    auto buffers = _col->getBuffers();
    if (!_col->isSubsetCollection()) {
      auto* dataVec = buffers.dataAsVector<float>();
      unsigned size = dataVec->size();
      device.data(size);
      podio::handlePODDataSIO(device, dataVec->data(), size);
    }

    // ---- references
    auto* refColls = buffers.references;
    for (auto& refC : *refColls) {
      unsigned size = refC->size();
      device.data(size);
      podio::handlePODDataSIO(device, refC->data(), size);
    }
  }

  void createCollection(const bool subsetCollection = false) override {
    setCollection(new AssociationCollection<FromT, ToT>());
    _col->setSubsetCollection(subsetCollection);
  }

  SIOBlock* create(const std::string& name) const override {
    return new AssociationSIOBlock(name);
  }
};

} // namespace podio

#endif // PODIO_ASSOCIATIONSIOBLOCK_H
