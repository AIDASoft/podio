#include "read_frame.h"
#include "read_frame_auxiliary.h"

#include "podio/ROOTFrameReader.h"

#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
  std::string inputFile = "example_frame.root";
  if (argc == 2) {
    inputFile = argv[1];
  }

  return read_frames<podio::ROOTFrameReader>(inputFile) + test_frame_aux_info<podio::ROOTFrameReader>(inputFile);
}
