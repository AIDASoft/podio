#include "write_frame_multithreaded.h"

#include "podio/ArrowWriter.h"

#include <cstdlib>
#include <filesystem>

int main(int argc, char* argv[]) {
  int nThreads = 4;
  int framesPerThread = 10;
  if (argc >= 2) {
    nThreads = std::atoi(argv[1]);
  }
  if (argc >= 3) {
    framesPerThread = std::atoi(argv[2]);
  }

  std::string filename = "example_frame_arrow_multithreaded.podio_parquet";
  std::filesystem::remove_all(filename);
  return write_frames_multithreaded<podio::ArrowWriter>(filename, nThreads, framesPerThread);
}
