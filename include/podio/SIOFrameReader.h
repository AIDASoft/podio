#ifndef PODIO_SIOFRAMEREADER_H
#define PODIO_SIOFRAMEREADER_H

#include "podio/SIOBlock.h"
#include "podio/SIORawData.h"
#include "podio/podioVersion.h"

#include "sio/api.h"

#include <memory>

namespace podio {

class CollectionIDTable;

class SIOFrameReader {

public:
  SIOFrameReader();
  ~SIOFrameReader() = default;

  /// Read all collections requested
  std::unique_ptr<podio::SIORawData> readNextEvent();

  unsigned getEntries() const {
    return m_tocRecord.getNRecords("event_record");
  }

  void openFile(const std::string& filename);

  podio::version::Version currentFileVersion() const {
    return m_fileVersion;
  }

private:
  /// read the TOC record
  bool readFileTOCRecord();

  void readCollectionIDTable();

  sio::ifstream m_stream{};

  // TODO: Move these somewhere else
  std::vector<std::string> m_typeNames{};
  std::vector<short> m_subsetCollectionBits{};

  std::shared_ptr<podio::CollectionIDTable> m_table{nullptr};
  unsigned m_eventNumber{0};

  SIOFileTOCRecord m_tocRecord{};
  podio::version::Version m_fileVersion{0};
};

} // namespace podio

#endif // PODIO_SIOFRAMEREADER_H
