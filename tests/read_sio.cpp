#include "podio/SIOReader.h"
#include "read_test.h"

int main() {
  //  auto reader = podio::SIOReader();
  podio::SIOReader reader; // SIOReader has no copy c'tor ...
  bool res = reader.openFile("example.sio");
  if (!res) {
    std::cout << "File could not be opened, aborting." << std::endl;
    return 1;
  }
  if (reader.currentFileVersion() != podio::version::build_version) {
    return 1;
  }

  run_read_test(reader);

  // jump back and forth a bit
  run_read_test_event(reader, 10);
  run_read_test_event(reader, 150);
  run_read_test_event(reader, 120);
  run_read_test_event(reader, 0);

  reader.closeFile();
  return 0;
}
