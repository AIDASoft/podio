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
    auto frame = podio::Frame(reader.readNextEntry("events"));
    if (frame.get("emptySubsetColl") == nullptr) {
      std::cerr << "Could not retrieve an empty subset collection" << std::endl;
      return 1;
    }
    if (frame.get("emptyCollection") == nullptr) {
      std::cerr << "Could not retrieve an empty collection" << std::endl;
      return 1;
    }

    processEvent(frame, i, reader.currentFileVersion());

    auto otherFrame = podio::Frame(reader.readNextEntry("other_events"));
    processEvent(otherFrame, i + 100, reader.currentFileVersion());
  }

  if (reader.readNextEntry("events")) {
    std::cerr << "Trying to read more frame data than is present should return a nullptr" << std::endl;
    return 1;
  }

  std::cout << "========================================================\n" << std::endl;
  if (reader.readNextEntry("not_present")) {
    std::cerr << "Trying to read non-existant frame data should return a nullptr" << std::endl;
    return 1;
  }

  // Reading specific (jumping to) entry
  {
    auto frame = podio::Frame(reader.readEntry("events", 4));
    processEvent(frame, 4, reader.currentFileVersion());
    // Reading the next entry after jump, continues from after the jump
    auto nextFrame = podio::Frame(reader.readNextEntry("events"));
    processEvent(nextFrame, 5, reader.currentFileVersion());

    auto otherFrame = podio::Frame(reader.readEntry("other_events", 4));
    processEvent(otherFrame, 4 + 100, reader.currentFileVersion());
    // Jumping back also works
    auto previousFrame = podio::Frame(reader.readEntry("other_events", 2));
    processEvent(previousFrame, 2 + 100, reader.currentFileVersion());

    // Trying to read a Frame that is not present returns a nullptr
    if (reader.readEntry("events", 10)) {
      std::cerr << "Trying to read a specific entry that does not exist should return a nullptr" << std::endl;
      return 1;
    }
  }

  return 0;
}

#endif // PODIO_TESTS_READ_FRAME_H
