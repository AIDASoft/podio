#ifndef PODIO_SIOFRAMEREADER_H
#define PODIO_SIOFRAMEREADER_H

#include "podio/SIOBlock.h"
#include "podio/SIOFrameData.h"
#include "podio/podioVersion.h"

#include <sio/definitions.h>

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

namespace podio {

class CollectionIDTable;

class SIOFrameReader {

public:
  SIOFrameReader();
  ~SIOFrameReader() = default;

  // non copyable
  SIOFrameReader(const SIOFrameReader&) = delete;
  SIOFrameReader& operator=(const SIOFrameReader&) = delete;

  /**
   * Read the next data entry from which a Frame can be constructed for the
   * given name. In case there are no more entries left for this name or in
   * case there is no data for this name, this returns a nullptr.
   */
  std::unique_ptr<podio::SIOFrameData> readNextEntry(const std::string& name);

  /**
   * Read the specified data entry from which a Frame can be constructed for
   * the given name. In case the entry does not exist for this name or in
   * case there is no data for this name, this returns a nullptr.
   */
  std::unique_ptr<podio::SIOFrameData> readEntry(const std::string& name, const unsigned entry);

  /// Returns number of entries for the given name
  unsigned getEntries(const std::string& name) const;

  void openFile(const std::string& filename);

  /// Get the build version of podio that has been used to write the current file
  podio::version::Version currentFileVersion() const {
    return m_fileVersion;
  }

  /// Get the names of all the availalable Frame categories in the current file(s)
  std::vector<std::string_view> getAvailableCategories() const;

private:
  void readPodioHeader();

  /// read the TOC record
  bool readFileTOCRecord();

  sio::ifstream m_stream{}; ///< The stream from which we read

  /// Count how many times each an entry of this name has been read already
  std::unordered_map<std::string, unsigned> m_nameCtr{};

  /// Table of content record where starting points of named entries can be read from
  SIOFileTOCRecord m_tocRecord{};
  /// The podio version that has been used to write the file
  podio::version::Version m_fileVersion{0};
};

} // namespace podio

#endif // PODIO_SIOFRAMEREADER_H
