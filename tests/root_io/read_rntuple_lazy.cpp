#include "read_frame.h"
#include "read_frame_auxiliary.h"

#include "podio/RNTupleLazyReader.h"

#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
  std::string inputFile = "example_rntuple.root";

  if (argc == 2) {
    inputFile = argv[1];
  } else if (argc > 2) {
    std::cerr << "Usage: " << argv[0] << " [input_file]" << std::endl;
    return 1;
  }

  return read_frames<podio::RNTupleLazyReader>(inputFile) +
      test_frame_aux_info<podio::RNTupleLazyReader>(inputFile) +
      test_read_frame_limited<podio::RNTupleLazyReader>(inputFile);
}
