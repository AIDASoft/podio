#include "write_interface.h"

#include <filesystem>

int main(int, char**) {

  std::filesystem::remove_all("example_frame_arrow_interface.podio_parquet");
  auto writerArrow = podio::makeWriter("example_frame_arrow_interface.podio_parquet", "parquet");
  write_frames(writerArrow);

  return 0;
}
