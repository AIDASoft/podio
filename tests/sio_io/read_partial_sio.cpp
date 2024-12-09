#include "read_partial.h"

#include "podio/SIOReader.h"

int main() {
  return read_partial_frames<podio::SIOReader>("example_frame.sio");
}
