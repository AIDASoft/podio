#include "write_frame.h"

#include "podio/ROOTWriter.h"

int main() {
  write_subsets_only<podio::ROOTWriter>("example_subsets_only.root");

  return 0;
}
