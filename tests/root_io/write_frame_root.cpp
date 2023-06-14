#include "write_frame.h"

#include "podio/ROOTFrameWriter.h"

int main(int, char**) {
  write_frames<podio::ROOTFrameWriter>("example_frame.root");
  return 0;
}
