#include "write_frame.h"

#include "podio/ArrowWriter.h"

#include <filesystem>

int main(int, char**) {
  std::string filename = "example_frame.podio_arrow";
  std::filesystem::remove_all(filename);
  write_frames<podio::ArrowWriter>(filename);
  return 0;
}
