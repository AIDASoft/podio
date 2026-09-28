#include "podio/Reader.h"
#include "read_frame.h"
#include "read_interface.h"

#include <string>

int main(int, char**) {
  auto readerArrow = podio::makeReader("example_frame_arrow_interface.podio_parquet");
  return read_frames(readerArrow) + test_read_frame_limited(readerArrow);
}
