#ifndef PODIO_SIOBLOCK_H
#define PODIO_SIOBLOCK_H

#include <podio/CollectionBase.h>
#include <podio/CollectionIDTable.h>
#include <podio/EventStore.h>
#include <podio/GenericParameters.h>
#include <podio/podioVersion.h>

#include <sio/block.h>
#include <sio/io_device.h>
#include <sio/version.h>

#include <map>
#include <memory>
#include <string>

namespace podio {

template <typename devT, typename PODData>
void handlePODDataSIO(devT& device, PODData* data, size_t size) {
  unsigned count = size * sizeof(PODData);
  char* dataPtr = reinterpret_cast<char*>(data);
  device.data(dataPtr, count);
}

/// Base class for sio::block handlers used with PODIO
class SIOBlock : public sio::block {

public:
  SIOBlock(const std::string& nam, sio::version_type vers) : sio::block(nam, vers) {
  }
  SIOBlock() = delete;
  SIOBlock(const SIOBlock&) = delete;
  SIOBlock& operator=(const SIOBlock&) = delete;

  podio::CollectionBase* getCollection() {
    return m_buffers.createCollection(m_buffers, m_subsetColl).release();
  }

  podio::CollectionReadBuffers getBuffers() const {
    return m_buffers;
  }

  std::string name() {
    return sio::block::name();
  }

  void setCollection(podio::CollectionBase* col) {
    m_subsetColl = col->isSubsetCollection();
    m_buffers = col->getBuffers();
  }

  virtual SIOBlock* create(const std::string& name) const = 0;

  // create a new collection for this block
  virtual void createBuffers(const bool subsetCollection = false) = 0;

protected:
  bool m_subsetColl{false};
  podio::CollectionReadBuffers m_buffers{};
};

/**
 * A dedicated block for handling the I/O of the CollectionIDTable
 */
class SIOCollectionIDTableBlock : public sio::block {
public:
  SIOCollectionIDTableBlock() : sio::block("CollectionIDs", sio::version::encode_version(0, 4)) {
  }

  SIOCollectionIDTableBlock(podio::EventStore* store);

  SIOCollectionIDTableBlock(const SIOCollectionIDTableBlock&) = delete;
  SIOCollectionIDTableBlock& operator=(const SIOCollectionIDTableBlock&) = delete;

  void read(sio::read_device& device, sio::version_type version) override;
  void write(sio::write_device& device) override;

  podio::CollectionIDTable* getTable() {
    return new podio::CollectionIDTable(std::move(_ids), std::move(_names));
  }
  const std::vector<std::string>& getTypeNames() const {
    return _types;
  }
  const std::vector<short>& getSubsetCollectionBits() const {
    return _isSubsetColl;
  }

private:
  std::vector<std::string> _names{};
  std::vector<int> _ids{};
  std::vector<std::string> _types{};
  std::vector<short> _isSubsetColl{};
};

struct SIOVersionBlock : public sio::block {
  SIOVersionBlock() : sio::block("podio_version", sio::version::encode_version(1, 0)) {
  }

  SIOVersionBlock(podio::version::Version v) :
      sio::block("podio_version", sio::version::encode_version(1, 0)), version(v) {
  }

  void write(sio::write_device& device) override {
    device.data(version);
  }

  void read(sio::read_device& device, sio::version_type) override {
    device.data(version);
  }

  podio::version::Version version{};
};

/**
 * A block for handling the EventMeta data
 */
class SIOEventMetaDataBlock : public sio::block {
public:
  SIOEventMetaDataBlock() : sio::block("EventMetaData", sio::version::encode_version(0, 1)) {
  }

  SIOEventMetaDataBlock(const SIOEventMetaDataBlock&) = delete;
  SIOEventMetaDataBlock& operator=(const SIOEventMetaDataBlock&) = delete;

  void read(sio::read_device& device, sio::version_type version) override;
  void write(sio::write_device& device) override;

  podio::GenericParameters* metadata{nullptr};
};

/**
 * A block for handling the run and collection meta data
 */
class SIONumberedMetaDataBlock : public sio::block {
public:
  SIONumberedMetaDataBlock(const std::string& name) : sio::block(name, sio::version::encode_version(0, 1)) {
  }

  SIONumberedMetaDataBlock(const SIONumberedMetaDataBlock&) = delete;
  SIONumberedMetaDataBlock& operator=(const SIONumberedMetaDataBlock&) = delete;

  void read(sio::read_device& device, sio::version_type version) override;
  void write(sio::write_device& device) override;

  std::map<int, GenericParameters>* data{nullptr};
};

/// factory for creating sio::blocks for a given type of EDM-collection
class SIOBlockFactory {
private:
  SIOBlockFactory() = default;

  typedef std::map<std::string, SIOBlock*> BlockMap;
  BlockMap _map{};

public:
  void registerBlockForCollection(const std::string& type, SIOBlock* b) {
    _map[type] = b;
  }

  std::shared_ptr<SIOBlock> createBlock(const podio::CollectionBase* col, const std::string& name) const;

  // return a block with a new collection (used for reading )
  std::shared_ptr<SIOBlock> createBlock(const std::string& typeStr, const std::string& name,
                                        const bool isRefColl = false) const;

  static SIOBlockFactory& instance() {
    static SIOBlockFactory me;
    return me;
  }
};

class SIOBlockLibraryLoader {
private:
  SIOBlockLibraryLoader();
  /**
   * Load a library with the given name via dlopen
   */
  void loadLib(const std::string& libname);
  /**
   * Get all files that are found on LD_LIBRARY_PATH and that have "SioBlocks"
   * in their name
   */
  static std::vector<std::string> getLibNames();

  std::map<std::string, void*> _loadedLibs{};

public:
  static SIOBlockLibraryLoader& instance() {
    static SIOBlockLibraryLoader instance;
    return instance;
  }
};

namespace sio_helpers {
  /// marker for showing that a TOC has been stored in the file
  static constexpr uint32_t SIOTocMarker = 0xc001fea7;
  /// the number of bits necessary to store the SIOTocMarker and the actual
  /// position of the start of the SIOFileTOCRecord
  static constexpr int SIOTocInfoSize = sizeof(uint64_t); // i.e. usually 8
  /// The name of the TOCRecord
  static constexpr const char* SIOTocRecordName = "podio_SIO_TOC_Record";

  // should hopefully be enough for all practical purposes
  using position_type = uint32_t;
} // namespace sio_helpers

class SIOFileTOCRecord {
public:
  using PositionType = sio_helpers::position_type;
  void addRecord(const std::string& name, PositionType startPos);

  size_t getNRecords(const std::string& name) const;

private:
  friend struct SIOFileTOCRecordBlock;

  using RecordListType = std::pair<std::string, std::vector<PositionType>>;
  using MapType = std::vector<RecordListType>;

  MapType m_recordMap{};
};

struct SIOFileTOCRecordBlock : public sio::block {
  SIOFileTOCRecordBlock() : sio::block(sio_helpers::SIOTocRecordName, sio::version::encode_version(0, 1)) {
  }

  SIOFileTOCRecordBlock(const SIOFileTOCRecordBlock&) = delete;
  SIOFileTOCRecordBlock& operator=(const SIOFileTOCRecordBlock&) = delete;

  void read(sio::read_device& device, sio::version_type version) override;
  void write(sio::write_device& device) override;

  SIOFileTOCRecord* record{nullptr};
};

} // namespace podio
#endif
