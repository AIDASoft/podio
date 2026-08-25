#include "read_frame.h"
#include "read_frame_auxiliary.h"

#include "podio/ArrowReader.h"

int main(int argc, char* argv[]) {
  std::string inputFile = "example_frame.podio_parquet";
  bool assertBuildVersion = true;
  if (argc == 2) {
    inputFile = argv[1];
    assertBuildVersion = false;
  }

  return read_frames<podio::ArrowReader>(inputFile, assertBuildVersion) +
      test_frame_aux_info<podio::ArrowReader>(inputFile) + test_read_frame_limited<podio::ArrowReader>(inputFile);
}
