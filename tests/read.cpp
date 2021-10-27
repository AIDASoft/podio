#include "read_test.h"
#include "podio/ROOTReader.h"

int main(){
  auto reader = podio::ROOTReader();
  reader.openFile("example.root");

  run_read_test(reader);

  // jump back and forth a bit
  run_read_test_event(reader, 10);
  run_read_test_event(reader, 150);
  run_read_test_event(reader, 120);

  reader.closeFile();
  return 0;
}
