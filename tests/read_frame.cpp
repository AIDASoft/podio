#include "podio/ROOTFrameReader.h"

#include "read_frame.h"

int main() {
  return read_frames<podio::ROOTFrameReader>("example_frame.root");
}
