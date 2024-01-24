#include "read_python_frame.h"

#include "podio/RNTupleReader.h"

int main() {
  return read_frame<podio::RNTupleReader>("example_frame_with_py_rntuple.root");
}
