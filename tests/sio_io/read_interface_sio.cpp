#include "read_interface.h"

int main(int, char**) {

  auto readerSIO = podio::makeReader("example_frame_sio_interface.sio");
  if (read_frames(readerSIO)) {
    return 1;
  }

  return 0;
}
