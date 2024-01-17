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

/**
 * A SIO reader for reading legacy podio .sio files that have been written using
 * the legacy, non Frame based I/O model. This reader grants Frame based access
 * to those files, by mimicking Frame I/O functionality and the interfaces of
 * those readers.
 *
 * NOTE: Since there was only one category ("events") for those legacy podio
 * files this reader will really only work if you try to read that category, and
 * will simply return no data if you try to read anything else.
 */
class SIOLegacyReader {

public:
  SIOLegacyReader();
  ~SIOLegacyReader() = default;

  // non copy-able
  SIOLegacyReader(const SIOLegacyReader&) = delete;
  SIOLegacyReader& operator=(const SIOLegacyReader&) = delete;

  /**
   * Read the next data entry from which a Frame can be constructed. In case
   * there are no more entries left, this returns a nullptr.
   *
   * NOTE: the category name has to be "events" in this case, as only that
   * category is available for legacy files.
   */
  std::unique_ptr<podio::SIOFrameData> readNextEntry(const std::string&);

  /**
   * Read the specified data entry from which a Frame can be constructed In case
   * the entry does not exist, this returns a nullptr.
   *
   * NOTE: the category name has to be "events" in this case, as only that
   * category is available for legacy files.
   */
  std::unique_ptr<podio::SIOFrameData> readEntry(const std::string&, const unsigned entry);

  /// Returns the number of
  unsigned getEntries(const std::string& name) const;

  void openFile(const std::string& filename);

  /// Get the build version of podio that has been used to write the current file
  podio::version::Version currentFileVersion() const {
    return m_fileVersion;
  }

  /// Get the names of all the available Frame categories in the current file(s)
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

#endif // PODIO_SIOLEGACYREADER_H
