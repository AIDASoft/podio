#ifndef PODIO_TESTS_READ_INTERFACE_H // NOLINT(llvm-header-guard): folder structure not suitable
#define PODIO_TESTS_READ_INTERFACE_H // NOLINT(llvm-header-guard): folder structure not suitable

#include "read_frame.h"

#include "podio/Reader.h"

int read_frames(podio::Reader& reader) {

  if (reader.getEntries(podio::Category::Event) != 10) {
    std::cerr << "Could not read back the number of events correctly. "
              << "(expected:" << 10 << ", actual: " << reader.getEntries(podio::Category::Event) << ")" << std::endl;
    return 1;
  }

  if (reader.getEntries(podio::Category::Event) != reader.getEntries("other_events")) {
    std::cerr << "Could not read back the number of events correctly. "
              << "(expected:" << 10 << ", actual: " << reader.getEntries("other_events") << ")" << std::endl;
    return 1;
  }

  // Read the frames in a different order than when writing them here to make
  // sure that the writing/reading order does not impose any usage requirements
  for (size_t i = 0; i < reader.getEntries(podio::Category::Event); ++i) {
    auto frame = reader.readNextFrame(podio::Category::Event);
    if (frame.get("emptySubsetColl") == nullptr) {
      std::cerr << "Could not retrieve an empty subset collection" << std::endl;
      return 1;
    }
    if (frame.get("emptyCollection") == nullptr) {
      std::cerr << "Could not retrieve an empty collection" << std::endl;
      return 1;
    }

    processEvent(frame, i, reader.currentFileVersion());

    auto otherFrame = reader.readNextFrame("other_events");
    processEvent(otherFrame, i + 100, reader.currentFileVersion());
    // The other_events category also holds external collections
    processExtensions(otherFrame, i + 100, reader.currentFileVersion());
    // As well as a test for the vector members subset category
    checkVecMemSubsetColl(otherFrame);
  }

  // if (reader.readNextFrame(podio::Category::Event)) {
  //   std::cerr << "Trying to read more frame data than is present should return a nullptr" << std::endl;
  //   return 1;
  // }

  std::cout << "========================================================\n" << std::endl;
  // if (reader.readNextFrame("not_present")) {
  //   std::cerr << "Trying to read non-existant frame data should return a nullptr" << std::endl;
  //   return 1;
  // }

  // Reading specific (jumping to) entry
  {
    auto frame = reader.readFrame(podio::Category::Event, 4);
    processEvent(frame, 4, reader.currentFileVersion());
    // Reading the next entry after jump, continues from after the jump
    auto nextFrame = reader.readNextFrame(podio::Category::Event);
    processEvent(nextFrame, 5, reader.currentFileVersion());

    auto otherFrame = reader.readFrame("other_events", 4);
    processEvent(otherFrame, 4 + 100, reader.currentFileVersion());
    if (reader.currentFileVersion() > podio::version::Version{0, 16, 2}) {
      processExtensions(otherFrame, 4 + 100, reader.currentFileVersion());
    }

    // Jumping back also works
    auto previousFrame = reader.readFrame("other_events", 2);
    processEvent(previousFrame, 2 + 100, reader.currentFileVersion());
    if (reader.currentFileVersion() > podio::version::Version{0, 16, 2}) {
      processExtensions(previousFrame, 2 + 100, reader.currentFileVersion());
    }

    // Trying to read a Frame that is not present returns a nullptr
    // if (reader.readFrame(podio::Category::Event, 10)) {
    //   std::cerr << "Trying to read a specific entry that does not exist should return a nullptr" << std::endl;
    //   return 1;
    // }
  }

  return 0;
}

/// Regression test for dangling string_view map keys in readers.
/// Uses a >15-char category (heap-allocated string) so ASAN quarantines
/// the freed buffer and detects the heap-use-after-free on the second
/// readFrame call that triggers find() on the stored dangling key.
int test_transient_category_strings(podio::Reader& reader) {
  if (reader.getEntries("events_extended") >= 2) {
    [&]() __attribute__((noinline)) {
      std::string category{"events_extended"};
      auto frame = reader.readFrame(category, 0);
      if (frame.getAvailableCollections().empty()) {
        std::cerr << "Expected non-empty collections for category 'events_extended'" << std::endl;
      }
    }();
    // String is now destroyed; heap buffer is in ASAN quarantine.
    // find() in readEntry reads the stored dangling key — ASAN flags this.
    auto frame = reader.readFrame(std::string{"events_extended"}, 1);
    if (frame.getAvailableCollections().empty()) {
      std::cerr << "Expected non-empty collections for category 'events_extended' entry 1" << std::endl;
      return 1;
    }
  }
  return 0;
}

#endif // PODIO_TESTS_READ_INTERFACE_H
