#include "selected_colls_roundtrip.h"

#include "podio/SIOReader.h"
#include "podio/SIOWriter.h"

int main() {
  return do_roundtrip<podio::SIOReader, podio::SIOWriter>("example_frame.sio", "selected_example_frame.sio");
}
