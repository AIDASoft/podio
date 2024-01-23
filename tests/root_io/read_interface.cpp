#include "read_frame.h"

#include "podio/ROOTFrameReader.h"
#include "podio/Reader.h"
#if PODIO_ENABLE_RNTUPLE
  #include "podio/RNTupleReader.h"
#endif

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

int main(int, char**) {

  auto reader = podio::makeReader("example_frame_interface.root");
  if (read_frames(reader)) {
    return 1;
  }

#if PODIO_ENABLE_RNTUPLE
  auto readerRNTuple = podio::makeReader("example_frame_rntuple_interface.root");
  if (read_frames(readerRNTuple)) {
    return 1;
  }
#endif

#if PODIO_ENABLE_SIO
  auto readerSIO = podio::makeReader("example_frame_sio_interface.sio");
  if (read_frames(readerSIO)) {
    return 1;
  }
#endif

  return 0;
}
