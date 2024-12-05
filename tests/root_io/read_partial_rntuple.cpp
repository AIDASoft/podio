#include "read_partial.h"

#include "podio/RNTupleReader.h"

int main() {
  return read_partial_frames<podio::RNTupleReader>("example_rntuple.root");
}
