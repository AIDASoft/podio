#include "read_python_frame.h"

#include "podio/ROOTNTupleReader.h"

int main() {
  return read_frame<podio::ROOTNTupleReader>("example_frame_with_py_rntuple.root");
}
