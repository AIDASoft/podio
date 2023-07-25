#ifndef PODIO_TESTS_READ_AND_WRITE_FRAME_H // NOLINT(llvm-header-guard): folder structure not suitable
#define PODIO_TESTS_READ_AND_WRITE_FRAME_H // NOLINT(llvm-header-guard): folder structure not suitable

#include "read_frame.h"

#include <string>

template <typename ReaderT, typename WriterT>
int rewrite_frames(const std::string& inputFile, const std::string& newOutput) {
  auto reader = ReaderT();
  reader.openFile(inputFile);

  auto writer = WriterT(newOutput);

  const auto frame = podio::Frame(reader.readEntry(podio::Category::Event, 0));
  writer.writeFrame(frame, podio::Category::Event);

  const auto otherFrame = podio::Frame(reader.readEntry("other_events", 0));
  writer.writeFrame(otherFrame, "other_events");

  return 0;
}

template <typename ReaderT>
int read_rewritten_frames(const std::string& inputName) {
  auto reader = ReaderT();
  reader.openFile(inputName);

  const auto frame = podio::Frame(reader.readEntry(podio::Category::Event, 0));
  processEvent(frame, 0, reader.currentFileVersion());

  const auto otherFrame = podio::Frame(reader.readEntry("other_events", 0));
  processEvent(otherFrame, 100, reader.currentFileVersion());

  return 0;
}

#endif // PODIO_TESTS_READ_AND_WRITE_FRAME_H
