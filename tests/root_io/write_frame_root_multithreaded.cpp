#include "write_frame_multithreaded.h"

#include "podio/ROOTWriter.h"

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

  return write_frames_multithreaded<podio::ROOTWriter>("example_frame_multithreaded.root", nThreads, framesPerThread);
}
