#include "write_frame.h"

#include "podio/ROOTWriter.h"

int main(int, char**) {
  write_frames<podio::ROOTWriter>("example_frame.root");
  return 0;
}
