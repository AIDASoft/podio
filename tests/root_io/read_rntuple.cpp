#include "podio/RNTupleReader.h"
#include "read_frame.h"

int main() {
  return read_frames<podio::RNTupleReader>("example_rntuple.root");
}
