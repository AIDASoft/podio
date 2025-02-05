#include "write_frame.h"

#include "podio/ROOTWriter.h"

#include <filesystem>

int main(int, char**) {
  const auto filename = "example_frame.root";
  write_frames<podio::ROOTWriter>(filename);
  // copy file multiple times for tests with glob
  std::filesystem::copy_file(filename, "example_frame_0.root", std::filesystem::copy_options::overwrite_existing);
  std::filesystem::copy_file(filename, "example_frame_1.root", std::filesystem::copy_options::overwrite_existing);
  return 0;
}
