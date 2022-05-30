#include "podio/Frame.h"
#include "podio/ROOTFrameReader.h"

#include "read_test.h"

int main() {
  auto reader = podio::ROOTFrameReader();
  reader.openFile("example_frame.root");

  if (reader.currentFileVersion() != podio::version::build_version) {
    return 1;
  }

  for (size_t i = 0; i < reader.getEntries("events"); ++i) {
    auto frame = podio::Frame(reader.readNextEvent("events"));
    processEvent(frame, i, reader.currentFileVersion());
  }

  for (size_t i = 0; i < reader.getEntries("other_events"); ++i) {
    auto frame = podio::Frame(reader.readNextEvent("other_events"));
    processEvent(frame, i + 100, reader.currentFileVersion());
  }

  return 0;
}
