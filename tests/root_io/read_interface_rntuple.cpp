#include "read_frame.h"
#include "read_interface.h"

int main(int, char**) {
  auto reader = podio::makeReader("example_from_rntuple_interface.root");
  return read_frames(reader) + test_read_frame_limited(reader);
}
