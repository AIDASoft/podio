#include "read_python_frame.h"

#include "podio/ROOTReader.h"

int main() {
  return read_frame<podio::ROOTReader>("example_frame_with_py.root");
}
