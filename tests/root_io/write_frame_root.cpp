#include "podio/ROOTWriter.h"
#include "write_frame.h"

#include <memory>

int main(int, char**) {
  write_frames<podio::ROOTWriter>("example_frame.root");
  return 0;
}
