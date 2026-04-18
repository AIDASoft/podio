#include "read_frame.h"
#include "read_frame_auxiliary.h"

#include "podio/ROOTLazyReader.h"

#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
  std::string inputFile = "example_frame.root";
  bool assertBuildVersion = true;
  if (argc == 2) {
    inputFile = argv[1];
    assertBuildVersion = false;
  } else if (argc > 2) {
    std::cout << "Wrong number of arguments" << std::endl;
    std::cout << "Usage: " << argv[0] << " FILE" << std::endl;
    return 1;
  }

  return read_frames<podio::ROOTLazyReader>(inputFile, assertBuildVersion) +
      test_frame_aux_info<podio::ROOTLazyReader>(inputFile) +
      test_read_frame_limited<podio::ROOTLazyReader>(inputFile);
}
