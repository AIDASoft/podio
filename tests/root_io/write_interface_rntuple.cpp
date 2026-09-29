#include "write_interface.h"

#include "podio/Writer.h"

int main(int, char**) {
  auto writer = podio::makeWriter("example_from_rntuple_interface.root", "rntuple");
  write_frames(writer);

  return 0;
}
