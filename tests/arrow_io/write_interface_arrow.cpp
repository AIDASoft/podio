#include "write_interface.h"

#include <filesystem>

int main(int, char**) {

  std::filesystem::remove_all("example_frame_arrow_interface.podio_arrow");
  auto writerArrow = podio::makeWriter("example_frame_arrow_interface.podio_arrow", "arrow");
  write_frames(writerArrow);

  return 0;
}
