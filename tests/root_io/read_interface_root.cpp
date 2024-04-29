#include "read_interface.h"

int main(int, char**) {

  auto reader = podio::makeReader("example_frame_interface.root");
  if (read_frames(reader)) {
    return 1;
  }

#if PODIO_ENABLE_RNTUPLE
  auto readerRNTuple = podio::makeReader("example_frame_rntuple_interface.root");
  if (read_frames(readerRNTuple)) {
    return 1;
  }
#endif

  return 0;
}
