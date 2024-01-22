#include "podio/RNTupleWriter.h"
#include "write_frame.h"

int main() {
  write_frames<podio::RNTupleWriter>("example_rntuple.root");
}
