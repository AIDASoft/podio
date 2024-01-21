#include "podio/ROOTRNTupleReader.h"
#include "read_frame.h"

int main() {
  return read_frames<podio::ROOTRNTupleReader>("example_rntuple.root");
}
