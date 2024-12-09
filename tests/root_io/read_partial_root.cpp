#include "read_partial.h"

#include "podio/ROOTReader.h"

int main() {
  return read_partial_frames<podio::ROOTReader>("example_frame.root");
}
