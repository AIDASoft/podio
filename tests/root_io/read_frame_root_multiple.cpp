#include "read_frame.h"

#include "podio/Frame.h"
#include "podio/ROOTReader.h"
#include "podio/ROOTWriter.h"
#include "podio/podioVersion.h"
#include "read_test.h"

#include <cstddef>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

int read_frames(podio::ROOTReader& reader) {
  if (reader.currentFileVersion() != podio::version::build_version) {
    std::cerr << "The podio build version could not be read back correctly. "
              << "(expected:" << podio::version::build_version << ", actual: " << reader.currentFileVersion() << ")"
              << std::endl;
    return 1;
  }

  if (reader.getEntries("events") != 20) {
    std::cerr << "Could not read back the number of events correctly. "
              << "(expected:" << 20 << ", actual: " << reader.getEntries("events") << ")" << std::endl;
    return 1;
  }

  if (reader.getEntries("events") != reader.getEntries("other_events")) {
    std::cerr << "Could not read back the number of events correctly. "
              << "(expected:" << 20 << ", actual: " << reader.getEntries("other_events") << ")" << std::endl;
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

    processEvent(frame, (i % 10), reader.currentFileVersion());

    auto otherFrame = podio::Frame(reader.readNextEntry("other_events"));
    processEvent(otherFrame, (i % 10) + 100, reader.currentFileVersion());
    // The other_events category also holds external collections
    processExtensions(otherFrame, (i % 10) + 100, reader.currentFileVersion());
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

    // Jump over a file boundary and make sure that works
    auto otherFrame = podio::Frame(reader.readEntry("other_events", 14));
    processEvent(otherFrame, 4 + 100, reader.currentFileVersion());
    processExtensions(otherFrame, 4 + 100, reader.currentFileVersion());

    // Jumping back also works
    auto previousFrame = podio::Frame(reader.readEntry("other_events", 2));
    processEvent(previousFrame, 2 + 100, reader.currentFileVersion());
    processExtensions(previousFrame, 2 + 100, reader.currentFileVersion());
  }

  // Trying to read a Frame that is not present returns a nullptr
  if (reader.readEntry("events", 30)) {
    std::cerr << "Trying to read a specific entry that does not exist should return a nullptr" << std::endl;
    return 1;
  }

  return 0;
}

// The following tests use freshly opened readers on which getEntries is never
// called. Depending on the backend getEntries might force some eager loading of
// information that might hide issues with the lazier loading when reading
// entries
int read_until_exhausted() {
  auto reader = podio::ROOTReader();
  reader.openFiles({"example_frame.root", "example_frame.root"});

  size_t nEntries = 0;
  while (auto frameData = reader.readNextEntry("events")) {
    auto frame = podio::Frame(std::move(frameData));
    processEvent(frame, (nEntries % 10), reader.currentFileVersion());
    nEntries++;
    if (nEntries > 20) {
      std::cerr << "Read more entries than are present (expected: 20)" << std::endl;
      return 1;
    }
  }

  if (nEntries != 20) {
    std::cerr << "Could not read all events via readNextEntry (expected: 20, actual: " << nEntries << ")" << std::endl;
    return 1;
  }

  if (reader.readNextEntry("events")) {
    std::cerr << "Trying to read more frame data than is present should return a nullptr" << std::endl;
    return 1;
  }

  return 0;
}

// Request an entry that is beyond the available entries while the TChain has
// not yet loaded the last file
int read_out_of_bounds_first() {
  {
    auto reader = podio::ROOTReader();
    reader.openFiles({"example_frame.root", "example_frame.root"});
    if (reader.readEntry("events", 30)) {
      std::cerr << "Trying to read a specific entry that does not exist (without having read any entry before) "
                   "should return a nullptr"
                << std::endl;
      return 1;
    }
  }
  {
    auto reader = podio::ROOTReader();
    reader.openFiles({"example_frame.root", "example_frame.root"});
    auto frame = podio::Frame(reader.readNextEntry("events"));
    processEvent(frame, 0, reader.currentFileVersion());
    if (reader.readEntry("events", 30)) {
      std::cerr << "Trying to read a specific entry that does not exist (after reading from the first file) should "
                   "return a nullptr"
                << std::endl;
      return 1;
    }

    // A failed attempt should not leave the reader in a broken state, i.e.
    // reading an existing entry from the last file has to still work
    auto lastFileFrame = podio::Frame(reader.readEntry("events", 15));
    processEvent(lastFileFrame, 5, reader.currentFileVersion());
    // And reading continues from there
    auto nextFrame = podio::Frame(reader.readNextEntry("events"));
    processEvent(nextFrame, 6, reader.currentFileVersion());
  }

  return 0;
}

// A failed attempt to read an entry beyond the available ones can make the
// TChain load other trees. Reading from the first file afterwards has to work,
// although the tree number is the same as for the last successful read
int read_out_of_bounds_then_first_file() {
  auto reader = podio::ROOTReader();
  reader.openFiles({"example_frame.root", "example_frame.root"});
  auto frame = podio::Frame(reader.readNextEntry("events"));
  processEvent(frame, 0, reader.currentFileVersion());
  if (reader.readEntry("events", 30)) {
    std::cerr << "Trying to read a specific entry that does not exist should return a nullptr" << std::endl;
    return 1;
  }
  auto firstFileFrame = podio::Frame(reader.readEntry("events", 3));
  processEvent(firstFileFrame, 3, reader.currentFileVersion());

  return 0;
}

// Requesting a collection that does not exist must not leave the reader in a
// broken state, even if the requested entry is in another file
int read_invalid_collection_other_file() {
  auto reader = podio::ROOTReader();
  reader.openFiles({"example_frame.root", "example_frame.root"});
  auto frame = podio::Frame(reader.readNextEntry("events"));
  processEvent(frame, 0, reader.currentFileVersion());
  try {
    reader.readEntry("events", 11, {"nonExistentCollection"});
    std::cerr << "Requesting a non-existent collection should throw" << std::endl;
    return 1;
  } catch (const std::invalid_argument&) {
  }
  auto otherFileFrame = podio::Frame(reader.readEntry("events", 12));
  processEvent(otherFileFrame, 2, reader.currentFileVersion());

  return 0;
}

// Reading only some collections for the first entry of a file must not break
// reading the other collections for the following entries
int read_subset_at_file_boundary() {
  auto reader = podio::ROOTReader();
  reader.openFiles({"example_frame.root", "example_frame.root"});
  for (size_t i = 0; i < 10; ++i) {
    reader.readNextEntry("events");
  }
  auto subsetFrame = podio::Frame(reader.readNextEntry("events", {"hits"}));
  if (subsetFrame.getAvailableCollections() != std::vector<std::string>{"hits"}) {
    std::cerr << "Reading a subset of collections should only provide these collections" << std::endl;
    return 1;
  }
  auto frame = podio::Frame(reader.readNextEntry("events"));
  processEvent(frame, 1, reader.currentFileVersion());

  return 0;
}

// Read random entries, including ones that do not exist, to cover arbitrary
// switches between the files in the chain
int read_random_access() {
  for (unsigned seed = 0; seed < 50; ++seed) {
    auto reader = podio::ROOTReader();
    reader.openFiles({"example_frame.root", "example_frame.root"});
    auto rng = std::mt19937(seed);
    auto entryDist = std::uniform_int_distribution<unsigned>(0, 24);
    for (size_t i = 0; i < 15; ++i) {
      const auto entry = entryDist(rng);
      auto frameData = reader.readEntry("events", entry);
      if (bool(frameData) != (entry < 20)) {
        std::cerr << "Entry " << entry << " should " << (entry < 20 ? "" : "not ") << "be available (seed " << seed
                  << ")" << std::endl;
        return 1;
      }
      if (frameData) {
        auto frame = podio::Frame(std::move(frameData));
        processEvent(frame, entry % 10, reader.currentFileVersion());
      }
    }
  }

  return 0;
}

// Files that are read together need to have the same contents. Make sure this
// is reported instead of crashing when the reader switches to a file that
// lacks some of the collections
int read_files_with_different_contents() {
  {
    auto reader = podio::ROOTReader();
    reader.openFile("example_frame.root");
    auto writer = podio::ROOTWriter("example_frame_hits_only.root");
    for (size_t i = 0; i < 10; ++i) {
      auto frame = podio::Frame(reader.readNextEntry("events"));
      writer.writeFrame(frame, "events", {"hits"});
    }
    writer.finish();
  }

  auto reader = podio::ROOTReader();
  reader.openFiles({"example_frame.root", "example_frame_hits_only.root"});
  for (size_t i = 0; i < 10; ++i) {
    reader.readNextEntry("events");
  }
  try {
    reader.readNextEntry("events");
    std::cerr << "Reading from a file that lacks some of the collections should throw" << std::endl;
    return 1;
  } catch (const std::runtime_error&) {
  }

  return 0;
}

int main() {
  auto reader = podio::ROOTReader();
  reader.openFiles({"example_frame.root", "example_frame.root"});

  return read_frames(reader) + read_until_exhausted() + read_out_of_bounds_first() +
      read_out_of_bounds_then_first_file() + read_invalid_collection_other_file() + read_subset_at_file_boundary() +
      read_random_access() + read_files_with_different_contents();
}
