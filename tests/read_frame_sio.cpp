#include "read_frame.h"
#include "read_frame_auxiliary.h"

#include "podio/SIOFrameReader.h"

int main() {
  return read_frames<podio::SIOFrameReader>("example_frame.sio") +
      test_frame_aux_info<podio::SIOFrameReader>("example_frame.sio");
}
