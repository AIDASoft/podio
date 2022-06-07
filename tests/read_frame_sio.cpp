#include "podio/Frame.h"
#include "podio/SIOFrameReader.h"

#include "read_test.h"

int main() {
  auto reader = podio::SIOFrameReader();
  reader.openFile("example_frame.sio");

  if (reader.currentFileVersion() != podio::version::build_version) {
    return 1;
  }

  if (reader.getEntries("events") != 10) {
    return 1;
  }

  if (reader.getEntries("events") != reader.getEntries("other_events")) {
    return 1;
  }

  for (size_t i = 0; i < reader.getEntries("events"); ++i) {
    auto frame = podio::Frame(reader.readNextFrame("events"));
    processEvent(frame, i, reader.currentFileVersion());

    auto otherFrame = podio::Frame(reader.readNextFrame("other_events"));
    processEvent(otherFrame, i + 100, reader.currentFileVersion());
  }

  return 0;
}
