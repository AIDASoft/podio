#include "podio/Frame.h"
#include "podio/ROOTFrameReader.h"

int main() {
  auto reader = podio::ROOTFrameReader();
  reader.openFile("example.root");

  if (reader.currentFileVersion() != podio::version::build_version) {
    return 1;
  }

  auto frame = podio::Frame(reader.readNextEvent());

  return 0;
}
