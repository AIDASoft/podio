#ifndef PODIO_SIOFRAMEREADER_H
#define PODIO_SIOFRAMEREADER_H

#include "podio/SIOBlock.h"
#include "podio/SIORawData.h"
#include "podio/podioVersion.h"

#include <sio/definitions.h>

#include <memory>
#include <string>
#include <unordered_map>

namespace podio {

class CollectionIDTable;

class SIOFrameReader {

public:
  SIOFrameReader();
  ~SIOFrameReader() = default;

  /// Read all collections requested
  std::unique_ptr<podio::SIORawData> readNextFrame(const std::string& category);

  unsigned getEntries(const std::string& category) const;

  void openFile(const std::string& filename);

  podio::version::Version currentFileVersion() const {
    return m_fileVersion;
  }

private:
  void readPodioHeader();

  /// read the TOC record
  bool readFileTOCRecord();

  sio::ifstream m_stream{}; ///< The stream from which we read

  /// Count how many times each category has been read already
  std::unordered_map<std::string, unsigned> m_categoryCtr{};

  /// Table of content record where starting points of categories can be read from
  SIOFileTOCRecord m_tocRecord{};
  /// The podio version that has been used to write the file
  podio::version::Version m_fileVersion{0};
};

} // namespace podio

#endif // PODIO_SIOFRAMEREADER_H
