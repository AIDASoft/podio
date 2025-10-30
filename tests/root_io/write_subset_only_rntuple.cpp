#include "write_frame.h"

#include "podio/RNTupleWriter.h"

int main() {
  write_subsets_only<podio::RNTupleWriter>("example_subsets_only_rntuple.root");

  return 0;
}
