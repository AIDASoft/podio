#ifndef PODIO_SIOWRITER_H
#define PODIO_SIOWRITER_H

#include "podio/CollectionBase.h"
#include "podio/EventStore.h"
#include "podio/SIOBlock.h"

// SIO specific includes
#include <sio/api.h>
#include <sio/definitions.h>
#include <sio/exception.h>

#include <iostream>
#include <string>
#include <vector>

// forward declarations

namespace podio {
class SIOWriter {

public:
  SIOWriter(const std::string& filename, EventStore* store);
  ~SIOWriter() = default;

  // non-copyable
  SIOWriter(const SIOWriter&) = delete;
  SIOWriter& operator=(const SIOWriter&) = delete;

  void registerForWrite(const std::string& name);
  void writeEvent();
  void finish();

private:
  void writeCollectionIDTable();
  sio::block_list createBlocks();

  // members
  std::string m_filename{};
  EventStore* m_store = nullptr;

  std::shared_ptr<SIOEventMetaDataBlock> m_eventMetaData;
  sio::ofstream m_stream{};
  sio::buffer m_buffer{sio::mbyte};
  sio::buffer m_com_buffer{sio::mbyte};
  bool m_firstEvent{true};

  std::shared_ptr<SIONumberedMetaDataBlock> m_runMetaData;
  std::shared_ptr<SIONumberedMetaDataBlock> m_collectionMetaData;
  SIOFileTOCRecord m_tocRecord{};
  std::vector<std::string> m_collectionsToWrite{};
};

} // namespace podio
#endif
