#include "selected_colls_roundtrip.h"

#include "podio/RNTupleReader.h"
#include "podio/RNTupleWriter.h"

int main() {
  return do_roundtrip<podio::RNTupleReader, podio::RNTupleWriter>("example_rntuple.root",
                                                                  "selected_example_rntuple.root");
}
