#include "podio/Frame.h"
#include "podio/ROOTFrameReader.h"

#include "read_test.h"

int main() {
  auto reader = podio::ROOTFrameReader();
  reader.openFile("example.root");

  if (reader.currentFileVersion() != podio::version::build_version) {
    return 1;
  }

  for (size_t i = 0; i < reader.getEntries(); ++i) {
    auto frame = podio::Frame(reader.readNextEvent());
    processEvent(frame, i, reader.currentFileVersion());
  }

  return 0;
}
