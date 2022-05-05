#include "podio/Frame.h"
#include "podio/SIOFrameReader.h"

#include "read_test.h"

int main() {
  auto reader = podio::SIOFrameReader();
  reader.openFile("example.sio");

  if (reader.currentFileVersion() != podio::version::build_version) {
    return 1;
  }

  for (size_t i = 0; i < reader.getEntries(); ++i) {
    auto frame = podio::Frame(reader.readNextEvent());
    processEvent(frame, i, reader.currentFileVersion());
  }

  return 0;
}
