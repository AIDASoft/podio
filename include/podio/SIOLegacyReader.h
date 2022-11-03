#ifndef PODIO_SIOLEGACYREADER_H
#define PODIO_SIOLEGACYREADER_H

#include "podio/SIOBlock.h"
#include "podio/SIOFrameData.h"
#include "podio/podioVersion.h"

#include <sio/definitions.h>

#include <memory>
#include <string_view>
#include <vector>

namespace podio {

class CollectionIDTable;

class SIOLegacyReader {

public:
  SIOLegacyReader();
  ~SIOLegacyReader() = default;

  /// Read all collections requested
  std::unique_ptr<podio::SIOFrameData> readNextEntry(const std::string&);

  /// Read the specified entry
  std::unique_ptr<podio::SIOFrameData> readEntry(const std::string&, const unsigned entry);

  unsigned getEntries(const std::string& name) const;

  void openFile(const std::string& filename);

  podio::version::Version currentFileVersion() const {
    return m_fileVersion;
  }

  std::vector<std::string_view> getAvailableCategories() const;

private:
  /// read the TOC record
  bool readFileTOCRecord();

  void readCollectionIDTable();

  sio::ifstream m_stream{};

  // TODO: Move these somewhere else
  std::vector<std::string> m_typeNames{};
  std::vector<short> m_subsetCollectionBits{};

  sio::buffer m_tableBuffer{1}; ///< The buffer holding the **compressed** CollectionIDTable
  unsigned m_tableUncLength{0}; ///< The uncompressed length of the tableBuffer

  std::shared_ptr<podio::CollectionIDTable> m_table{nullptr};
  unsigned m_eventNumber{0};

  SIOFileTOCRecord m_tocRecord{};
  podio::version::Version m_fileVersion{0};

  constexpr static auto m_categoryName = "events";
};

} // namespace podio

#endif // PODIO_SIOFRAMEREADER_H
