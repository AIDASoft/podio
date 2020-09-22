#include "read_test.h"
#include "podio/ROOTReader.h"

int main(){
  auto reader = podio::ROOTReader();
  reader.openFiles({"example.root", "example1.root"});

  run_read_test(reader);

  reader.closeFiles();
  return 0;
}
