#include "podio/ROOTNTupleReader.h"
#include "read_frame.h"

int main() {
  return read_frames<podio::ROOTNTupleReader>("example_rntuple.root");
}
