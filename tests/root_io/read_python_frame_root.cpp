#include "podio/ROOTReader.h"
#include "read_python_frame.h"

#include <memory>

int main() {
  return read_frame<podio::ROOTReader>("example_frame_with_py.root");
}
