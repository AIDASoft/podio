#ifndef PODIO_TESTS_READ_FRAME_H // NOLINT(llvm-header-guard): folder structure not suitable
#define PODIO_TESTS_READ_FRAME_H // NOLINT(llvm-header-guard): folder structure not suitable

#include "read_test.h"

#include "podio/Frame.h"

#include <iostream>

template <typename ReaderT>
int read_frames(const std::string& filename) {
  auto reader = ReaderT();
  reader.openFile(filename);

  if (reader.currentFileVersion() != podio::version::build_version) {
    std::cerr << "The podio build version could not be read back correctly. "
              << "(expected:" << podio::version::build_version << ", actual: " << reader.currentFileVersion() << ")"
              << std::endl;
    return 1;
  }

  if (reader.getEntries("events") != 10) {
    std::cerr << "Could not read back the number of events correctly. "
              << "(expected:" << 10 << ", actual: " << reader.getEntries("events") << ")" << std::endl;
    return 1;
  }

  if (reader.getEntries("events") != reader.getEntries("other_events")) {
    std::cerr << "Could not read back the number of events correctly. "
              << "(expected:" << 10 << ", actual: " << reader.getEntries("other_events") << ")" << std::endl;
    return 1;
  }

  // Read the frames in a different order than when writing them here to make
  // sure that the writing/reading order does not impose any usage requirements
  for (size_t i = 0; i < reader.getEntries("events"); ++i) {
    auto frame = podio::Frame(reader.readNextFrame("events"));
    processEvent(frame, i, reader.currentFileVersion());

    auto otherFrame = podio::Frame(reader.readNextFrame("other_events"));
    processEvent(otherFrame, i + 100, reader.currentFileVersion());
  }

  return 0;
}

#endif // PODIO_TESTS_READ_FRAME_H
