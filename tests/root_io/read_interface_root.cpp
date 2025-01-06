#include "read_frame.h"
#include "read_interface.h"

#include "podio/Reader.h"

int main(int, char**) {
  auto reader = podio::makeReader("example_frame_interface.root");
  return read_frames(reader) + test_read_frame_limited(reader);
}
