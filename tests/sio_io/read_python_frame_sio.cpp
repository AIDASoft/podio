#include "read_python_frame.h"

#include "podio/SIOReader.h"

int main() {
  return read_frame<podio::SIOReader>("example_frame_with_py.sio");
}
