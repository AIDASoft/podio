#include "podio/SIOFrameReader.h"

#include "read_frame.h"

int main() {
  read_frames<podio::SIOFrameReader>("example_frame.sio");
}
