#include "podio/ROOTReader.h"
#include "read_test.h"

#include <iostream>

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: read-legacy-files inputfile" << std::endl;
    return 1;
  }

  auto reader = podio::ROOTReader();
  reader.openFile(argv[1]);

  run_read_test(reader);

  reader.closeFile();
  return 0;
}
