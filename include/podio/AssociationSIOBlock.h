#ifndef PODIO_ASSOCIATIONSIOBLOCK_H
#define PODIO_ASSOCIATIONSIOBLOCK_H

#include "podio/AssociationCollection.h"
#include "podio/CollectionBufferFactory.h"

#include "podio/CollectionBuffers.h"
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
      SIOBlock(podio::detail::associationSIOName<FromT, ToT>(),
               sio::version::encode_version(AssociationCollection<FromT, ToT>::schemaVersion, 0)) {
    podio::SIOBlockFactory::instance().registerBlockForCollection(podio::detail::associationSIOName<FromT, ToT>(),
                                                                  this);
  }

  AssociationSIOBlock(const std::string& name) :
      SIOBlock(name, sio::version::encode_version(AssociationCollection<FromT, ToT>::schemaVersion, 0)) {
  }

  void read(sio::read_device& device, sio::version_type version) override {
    auto& bufferFactory = podio::CollectionBufferFactory::instance();
    // TODO:
    // - Error handling of empty optional
    auto maybeBuffers = bufferFactory.createBuffers(podio::detail::associationCollTypeName<FromT, ToT>(),
                                                    sio::version::major_version(version), m_subsetColl);
    m_buffers = maybeBuffers.value();

    if (!m_subsetColl) {
      unsigned size{0};
      device.data(size);
      auto* dataVec = m_buffers.dataAsVector<float>();
      dataVec->resize(size);
      podio::handlePODDataSIO(device, dataVec->data(), size);
    }

    // ---- read ref collections
    auto* refColls = m_buffers.references;
    for (auto& refC : *refColls) {
      unsigned size{0};
      device.data(size);
      refC->resize(size);
      podio::handlePODDataSIO(device, refC->data(), size);
    }
  }

  void write(sio::write_device& device) override {
    if (!m_subsetColl) {
      auto* dataVec = podio::CollectionWriteBuffers::asVector<float>(m_buffers.data);
      unsigned size = dataVec->size();
      device.data(size);
      podio::handlePODDataSIO(device, dataVec->data(), size);
    }

    // ---- write ref collections ------
    auto* refColls = m_buffers.references;
    for (auto& refC : *refColls) {
      unsigned size = refC->size();
      device.data(size);
      podio::handlePODDataSIO(device, refC->data(), size);
    }
  }

  SIOBlock* create(const std::string& name) const override {
    return new AssociationSIOBlock(name);
  }
};

} // namespace podio

#endif // PODIO_ASSOCIATIONSIOBLOCK_H
