#include "podio/RNTupleReader.h"
#include "read_python_frame.h"

#include <memory>

int main() {
  return read_frame<podio::RNTupleReader>("example_frame_with_py_rntuple.root");
}
