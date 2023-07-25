#include "read_python_frame.h"

#include "podio/ROOTFrameReader.h"

int main() {
  return read_frame<podio::ROOTFrameReader>("example_frame_with_py.root");
}
