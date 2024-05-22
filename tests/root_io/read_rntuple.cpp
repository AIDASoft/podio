#include "podio/RNTupleReader.h"
#include "read_frame.h"

#include <memory>

int main() {
  return read_frames<podio::RNTupleReader>("example_rntuple.root");
}
