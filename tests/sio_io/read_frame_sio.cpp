#include "read_frame.h"
#include "read_frame_auxiliary.h"

#include "podio/SIOReader.h"

int main(int argc, char* argv[]) {
  std::string inputFile = "example_frame.sio";
  bool assertBuildVersion = true;
  if (argc == 2) {
    inputFile = argv[1];
    assertBuildVersion = false;
  }

  return read_frames<podio::SIOReader>(inputFile, assertBuildVersion) +
      test_frame_aux_info<podio::SIOReader>(inputFile) + test_read_frame_limited<podio::SIOReader>(inputFile);
}
