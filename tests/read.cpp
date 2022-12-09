#include "podio/ROOTReader.h"
#include "read_test.h"

int main() {
  auto reader = podio::ROOTReader();
  try {
    reader.openFile("example.root");
  }
  catch (const std::runtime_error& e) {
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
