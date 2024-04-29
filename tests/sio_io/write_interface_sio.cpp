#include "write_interface.h"

int main(int, char**) {

  auto writerSIO = podio::makeWriter("example_frame_sio_interface.sio", "sio");
  write_frames(writerSIO);

  return 0;
}
