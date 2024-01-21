#include "read_python_frame.h"

#include "podio/ROOTRNTupleReader.h"

int main() {
  return read_frame<podio::ROOTRNTupleReader>("example_frame_with_py_rntuple.root");
}
