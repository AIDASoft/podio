#include "read_python_frame.h"

#include "podio/SIOFrameReader.h"

int main() {
  return read_frame<podio::SIOFrameReader>("example_frame_with_py.sio");
}
