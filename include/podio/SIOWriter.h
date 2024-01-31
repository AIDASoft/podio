#ifndef PODIO_SIOWRITER_H
#define PODIO_SIOWRITER_H

#include "podio/SIOBlock.h"
#include "podio/utilities/DatamodelRegistryIOHelpers.h"

#include <sio/definitions.h>

#include <string>
#include <utility>
#include <vector>

namespace podio {

class Frame;

class SIOWriter {
public:
  SIOWriter(const std::string& filename);
  ~SIOWriter();

  SIOWriter(const SIOWriter&) = delete;
  SIOWriter& operator=(const SIOWriter&) = delete;

  /** Write the given Frame with the given category
   */
  void writeFrame(const podio::Frame& frame, const std::string& category);

  /** Write the given Frame with the given category only storing the collections
   * that are desired via collsToWrite
   */
  void writeFrame(const podio::Frame& frame, const std::string& category, const std::vector<std::string>& collsToWrite);

  void finish();

private:
  sio::ofstream m_stream{};       ///< The output file stream
  SIOFileTOCRecord m_tocRecord{}; ///< The "table of contents" of the written file
  DatamodelDefinitionCollector m_datamodelCollector{};
  bool m_finished{false}; ///< Has finish been called already?
};
} // namespace podio

#endif // PODIO_SIOWRITER_H
