#include "read_frame_multithreaded.h"

#include "podio/RNTupleReader.h"

#include <cstdlib>

int main(int argc, char* argv[]) {
  int nThreads = 4;
  int framesPerThread = 10;
  if (argc >= 2) {
    nThreads = std::atoi(argv[1]);
  }
  if (argc >= 3) {
    framesPerThread = std::atoi(argv[2]);
  }

  const unsigned expectedEntries = nThreads * framesPerThread;
  return read_frames_multithreaded<podio::RNTupleReader>("example_rntuple_multithreaded.root", nThreads,
                                                         expectedEntries);
}
