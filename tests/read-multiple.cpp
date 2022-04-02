#include "podio/ROOTReader.h"
#include "read_test.h"

int main() {
  auto reader = podio::ROOTReader();
  reader.openFiles({"example.root", "example1.root"});

  run_read_test(reader);

  reader.closeFiles();
  return 0;
}
