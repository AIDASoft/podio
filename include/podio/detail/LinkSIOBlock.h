#ifndef PODIO_DETAIL_LINKSIOBLOCK_H
#define PODIO_DETAIL_LINKSIOBLOCK_H

#include "podio/detail/LinkCollectionImpl.h"

#include "podio/CollectionBufferFactory.h"
#include "podio/CollectionBuffers.h"
#include "podio/SIOBlock.h"

#include <sio/api.h>
#include <sio/io_device.h>
#include <sio/version.h>

#include <string>

namespace podio {
template <typename FromT, typename ToT>
class LinkSIOBlock : public podio::SIOBlock {
public:
  LinkSIOBlock() :
      SIOBlock(podio::detail::linkSIOName<FromT, ToT>(),
               sio::version::encode_version(LinkCollection<FromT, ToT>::schemaVersion, 0)) {
    podio::SIOBlockFactory::instance().registerBlockForCollection(
        std::string(podio::detail::linkTypeName<FromT, ToT>()), this);
  }

  LinkSIOBlock(const std::string& name) :
      SIOBlock(name, sio::version::encode_version(LinkCollection<FromT, ToT>::schemaVersion, 0)) {
  }

  void read(sio::read_device& device, sio::version_type version) override {
    auto& bufferFactory = podio::CollectionBufferFactory::instance();
    // TODO:
    // - Error handling of empty optional
    auto maybeBuffers = bufferFactory.createBuffers(std::string(podio::detail::linkCollTypeName<FromT, ToT>()),
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
    return new LinkSIOBlock(name);
  }
};

} // namespace podio

#endif // PODIO_DETAIL_LINKSIOBLOCK_H
