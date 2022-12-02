#ifndef PODIO_ASSOCIATIONSIOBLOCK_H
#define PODIO_ASSOCIATIONSIOBLOCK_H

#include "podio/detail/AssociationFwd.h"

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
      SIOBlock(podio::detail::associationSIOName<FromT, ToT>(), sio::version::encode_version(0, 1)) {
    podio::SIOBlockFactory::instance().registerBlockForCollection(podio::detail::associationSIOName<FromT, ToT>(),
                                                                  this);
  }

  AssociationSIOBlock(const std::string& name) : SIOBlock(name, sio::version::encode_version(0, 1)) {
  }

  void read(sio::read_device& device, sio::version_type) override {
    m_buffers.references->emplace_back(std::make_unique<std::vector<podio::ObjectID>>());
    if (!m_subsetColl) {
      m_buffers.references->emplace_back(std::make_unique<std::vector<podio::ObjectID>>());
    }

    if (!m_subsetColl) {
      unsigned size{0};
      device.data(size);
      m_buffers.data = new std::vector<float>(size);
      auto* dataVec = m_buffers.dataAsVector<float>();
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

    // ---- wirte ref collections ------
    auto* refColls = m_buffers.references;
    for (auto& refC : *refColls) {
      unsigned size = refC->size();
      device.data(size);
      podio::handlePODDataSIO(device, refC->data(), size);
    }
  }

  void createBuffers(const bool subsetCollection = false) override {
    m_subsetColl = subsetCollection;

    m_buffers.references = new podio::CollRefCollection();
    m_buffers.vectorMembers = new podio::VectorMembersInfo();

    m_buffers.createCollection = [](podio::CollectionReadBuffers buffers, bool isSubsetColl) {
      AssociationCollectionData<FromT, ToT> data(buffers, isSubsetColl);
      return std::make_unique<AssociationCollection<FromT, ToT>>(std::move(data), isSubsetColl);
    };

    // setCollection(new AssociationCollection<FromT, ToT>());
    // _col->setSubsetCollection(subsetCollection);
  }

  SIOBlock* create(const std::string& name) const override {
    return new AssociationSIOBlock(name);
  }
};

} // namespace podio

#endif // PODIO_ASSOCIATIONSIOBLOCK_H
