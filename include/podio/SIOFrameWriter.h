#ifndef PODIO_SIOFRAMEWRITER_H
#define PODIO_SIOFRAMEWRITER_H

#include "podio/SIOBlock.h"

#include <sio/definitions.h>

#include <string>
#include <utility>
#include <vector>

namespace podio {

class Frame;

class SIOFrameWriter {
public:
  SIOFrameWriter(const std::string& filename);
  ~SIOFrameWriter() = default;

  SIOFrameWriter(const SIOFrameWriter&) = delete;
  SIOFrameWriter& operator=(const SIOFrameWriter&) = delete;

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
};
} // namespace podio

#endif // PODIO_SIOFRAMEWRITER_H
