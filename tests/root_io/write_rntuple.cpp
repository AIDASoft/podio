#include "podio/ROOTRNTupleWriter.h"
#include "write_frame.h"

int main() {
  write_frames<podio::ROOTRNTupleWriter>("example_rntuple.root");
}
