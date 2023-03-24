#include "write_frame.h"

#include "podio/SIOFrameWriter.h"

int main(int, char**) {
  write_frames<podio::SIOFrameWriter>("example_frame.sio");
  return 0;
}
