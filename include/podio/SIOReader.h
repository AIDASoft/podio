#ifndef PODIO_SIOREADER_H
#define PODIO_SIOREADER_H

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "podio/EventStore.h"
#include "podio/ICollectionProvider.h"
#include "podio/IReader.h"
#include "podio/SIOBlock.h"

// -- sio headers
#include <sio/api.h>
#include <sio/buffer.h>
#include <sio/definitions.h>
#include <sio/exception.h>

namespace podio {

class CollectionBase;
class CollectionIDTable;

/**
   This class has the function to read available data from disk
   and to prepare collections and buffers.
**/
class SIOReader : public IReader {
  friend EventStore;

public:
  SIOReader();
  ~SIOReader() = default;

  // make non-copyable
  SIOReader(const SIOReader&) = delete;
  SIOReader& operator=(const SIOReader&) = delete;

  bool openFile(const std::string& filename) override;
  void closeFile() override;

  /// Read all collections requested
  void readEvent() override;

  void goToEvent(unsigned iEvent) override;

  /// Read CollectionIDTable from SIO file
  std::shared_ptr<CollectionIDTable> getCollectionIDTable() override {
    return m_table;
  }

  unsigned getEntries() const override {
    return m_tocRecord.getNRecords("event_record");
  }

  /// Check if file is valid
  bool isValid() const override;

  podio::version::Version currentFileVersion() const override {
    return m_fileVersion;
  }

  void endOfEvent() override;

private:
  /// Implementation for collection reading
  CollectionBase* readCollection(const std::string& name) override;

  /// read event meta data for current event
  GenericParameters* readEventMetaData() override;

  /// read the collection meta data
  std::map<int, GenericParameters>* readCollectionMetaData() override;

  /// read the run meta data
  std::map<int, GenericParameters>* readRunMetaData() override;

  /// read the TOC record
  bool readFileTOCRecord();

  /// reconstruct the TOC record from the file contents
  void reconstructFileTOCRecord();

private:
  void readCollectionIDTable();
  void readMetaDataRecord(const std::shared_ptr<SIONumberedMetaDataBlock>& mdBlock);
  void createBlocks();

  typedef std::pair<CollectionBase*, std::string> Input;
  std::vector<Input> m_inputs{};
  std::shared_ptr<CollectionIDTable> m_table{nullptr}; // Co-owned by the EventStore
  int m_eventNumber{0};
  int m_lastEventRead{-1};
  std::vector<std::string> m_typeNames{};
  std::vector<short> m_subsetCollectionBits{};

  std::shared_ptr<SIOEventMetaDataBlock> m_eventMetaData{};
  std::shared_ptr<SIONumberedMetaDataBlock> m_runMetaData{};
  std::shared_ptr<SIONumberedMetaDataBlock> m_collectionMetaData{};

  sio::ifstream m_stream{};
  sio::record_info m_rec_info{};
  sio::buffer m_info_buffer{sio::max_record_info_len};
  sio::buffer m_rec_buffer{sio::mbyte};
  sio::buffer m_unc_buffer{sio::mbyte};
  sio::block_list m_blocks{};

  SIOFileTOCRecord m_tocRecord{};

  podio::version::Version m_fileVersion{0};
};

} // namespace podio

#endif
