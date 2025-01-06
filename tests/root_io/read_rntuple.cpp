#include "read_frame.h"
#include "read_frame_auxiliary.h"

#include "podio/RNTupleReader.h"

int main() {
  const std::string inputFile = "example_rntuple.root";

  return read_frames<podio::RNTupleReader>(inputFile) + test_frame_aux_info<podio::RNTupleReader>(inputFile);
}
