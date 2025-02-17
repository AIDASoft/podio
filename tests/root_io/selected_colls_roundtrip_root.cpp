#include "selected_colls_roundtrip.h"

#include "podio/ROOTReader.h"
#include "podio/ROOTWriter.h"

int main() {
  return do_roundtrip<podio::ROOTReader, podio::ROOTWriter>("example_frame.root", "selected_example_frame.root");
}
