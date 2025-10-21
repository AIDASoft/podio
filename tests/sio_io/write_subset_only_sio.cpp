#include "write_frame.h"

#include "podio/SIOWriter.h"

int main() {
  write_subsets_only<podio::SIOWriter>("example_subsets_only.sio");

  return 0;
}
