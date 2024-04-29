#include "write_interface.h"

int main(int, char**) {

  auto writer = podio::makeWriter("example_frame_interface.root");
  write_frames(writer);

#if PODIO_ENABLE_RNTUPLE
  auto writerRNTuple = podio::makeWriter("example_frame_rntuple_interface.root", "rntuple");
  write_frames(writerRNTuple);
#endif

  return 0;
}
