#include "read_frame.h"
#include "read_interface.h"

int main(int, char**) {
  auto readerSIO = podio::makeReader("example_frame_sio_interface.sio");
  return read_frames(readerSIO) + test_read_frame_limited(readerSIO);

  return 0;
}
